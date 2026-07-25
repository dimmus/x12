//! Memory-safe X12 protocol front-end (ROADMAP step 6 / ADR-0011).
//!
//! Decodes and validates untrusted request byte streams for the native
//! **X12-SURFACE** path before they reach the C server core. Hierarchical
//! client levels (ADR-0003) are enforced here as a policy gate.
//!
//! This is a spike: not yet wired into the Xvfb request loop. The C ABI in
//! [`ffi`] is the intended integration surface for step 7+.

#![deny(unsafe_op_in_unsafe_fn)]

pub mod ffi;
pub mod frame;
pub mod level;
pub mod x12_surface;

pub use frame::{DecodeError, RequestHeader};
pub use level::ClientLevel;
pub use x12_surface::{DecodedSurfaceRequest, SurfaceOpcode};

/// Result of decoding one client request through the safe front-end.
#[derive(Debug, Clone, PartialEq, Eq)]
pub enum DecodedRequest {
    X12Surface(DecodedSurfaceRequest),
}

/// Decode + validate a single request buffer for extension major `major_opcode`.
///
/// `fds_attached` is the count of SCM_RIGHTS file descriptors delivered with
/// this request (dmabuf / syncobj planes). The byte stream itself does not
/// contain FD payloads.
pub fn decode_request(
    major_opcode: u8,
    buf: &[u8],
    fds_attached: u32,
    level: ClientLevel,
) -> Result<DecodedRequest, DecodeError> {
    let header = RequestHeader::parse(buf)?;
    if header.major_opcode != major_opcode {
        return Err(DecodeError::MajorMismatch {
            expected: major_opcode,
            got: header.major_opcode,
        });
    }
    let body = header.body(buf)?;
    let decoded = x12_surface::decode(header.minor_opcode, body, fds_attached)?;
    level::authorize_surface(&decoded, level)?;
    Ok(DecodedRequest::X12Surface(decoded))
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::x12_surface::SurfaceOpcode;

    fn put_u16(buf: &mut Vec<u8>, v: u16) {
        buf.extend_from_slice(&v.to_le_bytes());
    }
    fn put_u32(buf: &mut Vec<u8>, v: u32) {
        buf.extend_from_slice(&v.to_le_bytes());
    }
    fn put_u64(buf: &mut Vec<u8>, v: u64) {
        buf.extend_from_slice(&v.to_le_bytes());
    }

    fn with_header(major: u8, minor: u8, body: &[u8]) -> Vec<u8> {
        assert_eq!(body.len() % 4, 0);
        let units = ((4 + body.len()) / 4) as u16;
        let mut out = Vec::with_capacity(4 + body.len());
        out.push(major);
        out.push(minor);
        put_u16(&mut out, units);
        out.extend_from_slice(body);
        out
    }

    #[test]
    fn query_version_ok() {
        let mut body = Vec::new();
        put_u32(&mut body, 1);
        put_u32(&mut body, 0);
        let buf = with_header(140, SurfaceOpcode::QueryVersion as u8, &body);
        let d = decode_request(140, &buf, 0, ClientLevel::Sandbox).unwrap();
        match d {
            DecodedRequest::X12Surface(DecodedSurfaceRequest::QueryVersion {
                major_version,
                minor_version,
            }) => {
                assert_eq!((major_version, minor_version), (1, 0));
            }
            other => panic!("unexpected {other:?}"),
        }
    }

    #[test]
    fn create_surface_rejects_bad_plane_count() {
        let mut body = Vec::new();
        put_u32(&mut body, 0x100); // surface
        put_u32(&mut body, 0x200); // drawable
        body.push(0); // num_buffers illegal
        body.extend_from_slice(&[0, 0, 0]);
        put_u16(&mut body, 64);
        put_u16(&mut body, 64);
        for _ in 0..8 {
            put_u32(&mut body, 0);
        }
        body.push(24);
        body.push(32);
        body.extend_from_slice(&[0, 0]);
        put_u64(&mut body, 0);
        put_u32(&mut body, 0x34325258); // XR24
        put_u32(&mut body, 0);
        let buf = with_header(140, SurfaceOpcode::CreateSurface as u8, &body);
        let err = decode_request(140, &buf, 0, ClientLevel::Full).unwrap_err();
        assert!(matches!(err, DecodeError::InvalidField(_)));
    }

    #[test]
    fn create_surface_requires_matching_fds() {
        let mut body = Vec::new();
        put_u32(&mut body, 0x100);
        put_u32(&mut body, 0x200);
        body.push(2);
        body.extend_from_slice(&[0, 0, 0]);
        put_u16(&mut body, 64);
        put_u16(&mut body, 64);
        // plane0/1 used; plane2/3 must be zero
        put_u32(&mut body, 256); // stride0
        put_u32(&mut body, 0); // offset0
        put_u32(&mut body, 256); // stride1
        put_u32(&mut body, 0); // offset1
        for _ in 0..4 {
            put_u32(&mut body, 0);
        }
        body.push(24);
        body.push(32);
        body.extend_from_slice(&[0, 0]);
        put_u64(&mut body, 0);
        put_u32(&mut body, 0x34325258);
        put_u32(&mut body, 0);
        let buf = with_header(140, SurfaceOpcode::CreateSurface as u8, &body);
        assert!(decode_request(140, &buf, 1, ClientLevel::Full).is_err());
        let d = decode_request(140, &buf, 2, ClientLevel::User).unwrap();
        match d {
            DecodedRequest::X12Surface(DecodedSurfaceRequest::CreateSurface {
                num_buffers,
                width,
                height,
                format,
                ..
            }) => {
                assert_eq!(num_buffers, 2);
                assert_eq!((width, height), (64, 64));
                assert_eq!(format, 0x34325258);
            }
            other => panic!("unexpected {other:?}"),
        }
    }

    #[test]
    fn sandbox_denied_present() {
        let mut body = Vec::new();
        put_u32(&mut body, 0x10); // window
        put_u32(&mut body, 0x20); // surface
        put_u32(&mut body, 1); // serial
        body.extend_from_slice(&[0u8; 8]);
        put_u16(&mut body, 0);
        put_u16(&mut body, 0);
        body.extend_from_slice(&[0u8; 4]);
        put_u32(&mut body, 0); // acquire
        put_u32(&mut body, 0); // release
        put_u64(&mut body, 0);
        put_u64(&mut body, 0);
        put_u32(&mut body, 0);
        body.extend_from_slice(&[0u8; 4]);
        put_u64(&mut body, 0);
        put_u64(&mut body, 0);
        put_u64(&mut body, 0);
        let buf = with_header(140, SurfaceOpcode::Present as u8, &body);
        let err = decode_request(140, &buf, 0, ClientLevel::Sandbox).unwrap_err();
        assert!(matches!(err, DecodeError::DeniedByLevel { .. }));
        assert!(decode_request(140, &buf, 0, ClientLevel::Full).is_ok());
    }

    #[test]
    fn truncated_request_fails() {
        let buf = [140u8, 0, 2, 0]; // claims 8 bytes, only 4 present
        let err = decode_request(140, &buf, 0, ClientLevel::Full).unwrap_err();
        assert!(matches!(err, DecodeError::Truncated { .. }));
    }

    #[test]
    fn query_version_rejects_stray_fds() {
        let mut body = Vec::new();
        put_u32(&mut body, 1);
        put_u32(&mut body, 0);
        let buf = with_header(140, SurfaceOpcode::QueryVersion as u8, &body);
        let err = decode_request(140, &buf, 1, ClientLevel::Full).unwrap_err();
        assert!(matches!(err, DecodeError::FdCountMismatch { .. }));
    }

    fn present_body() -> Vec<u8> {
        let mut body = Vec::new();
        put_u32(&mut body, 0x10);
        put_u32(&mut body, 0x20);
        put_u32(&mut body, 7);
        body.extend_from_slice(&[0u8; 8]);
        put_u16(&mut body, 1u16); // x_off as bits
        put_u16(&mut body, 0xffffu16); // y_off = -1
        body.extend_from_slice(&[0u8; 4]);
        put_u32(&mut body, 0x30);
        put_u32(&mut body, 0x40);
        put_u64(&mut body, 11);
        put_u64(&mut body, 12);
        put_u32(&mut body, 0b100); // TearFree
        body.extend_from_slice(&[0u8; 4]);
        put_u64(&mut body, 100);
        put_u64(&mut body, 1);
        put_u64(&mut body, 0);
        body
    }

    #[test]
    fn present_ok_and_sizes() {
        let body = present_body();
        assert_eq!(body.len(), 84);
        let buf = with_header(140, SurfaceOpcode::Present as u8, &body);
        assert_eq!(buf.len(), 88); // X12_PROTO_SZ_PRESENT_REQ
        let d = decode_request(140, &buf, 0, ClientLevel::Full).unwrap();
        match d {
            DecodedRequest::X12Surface(DecodedSurfaceRequest::Present {
                serial,
                y_off,
                acquire_point,
                options,
                ..
            }) => {
                assert_eq!(serial, 7);
                assert_eq!(y_off, -1);
                assert_eq!(acquire_point, 11);
                assert_eq!(options, 0b100);
            }
            other => panic!("unexpected {other:?}"),
        }
    }

    #[test]
    fn import_syncobj_requires_one_fd() {
        let mut body = Vec::new();
        put_u32(&mut body, 0x55);
        put_u32(&mut body, 0x66);
        let buf = with_header(140, SurfaceOpcode::ImportSyncobj as u8, &body);
        assert_eq!(buf.len(), 12);
        assert!(decode_request(140, &buf, 0, ClientLevel::Full).is_err());
        let d = decode_request(140, &buf, 1, ClientLevel::User).unwrap();
        match d {
            DecodedRequest::X12Surface(DecodedSurfaceRequest::ImportSyncobj {
                syncobj,
                drawable,
            }) => {
                assert_eq!((syncobj, drawable), (0x55, 0x66));
            }
            other => panic!("unexpected {other:?}"),
        }
    }

    #[test]
    fn ffi_fills_create_surface_out() {
        let mut body = Vec::new();
        put_u32(&mut body, 0x100);
        put_u32(&mut body, 0x200);
        body.push(1);
        body.extend_from_slice(&[0, 0, 0]);
        put_u16(&mut body, 32);
        put_u16(&mut body, 48);
        put_u32(&mut body, 128);
        put_u32(&mut body, 0);
        for _ in 0..6 {
            put_u32(&mut body, 0);
        }
        body.push(24);
        body.push(32);
        body.extend_from_slice(&[0, 0]);
        put_u64(&mut body, 9);
        put_u32(&mut body, 0x34325258);
        put_u32(&mut body, 0);
        let buf = with_header(140, SurfaceOpcode::CreateSurface as u8, &body);
        assert_eq!(buf.len(), 72);
        let mut out = core::mem::MaybeUninit::<ffi::X12ProtoSurfaceDecoded>::uninit();
        let rc = unsafe {
            ffi::x12_proto_decode_surface(
                140,
                buf.as_ptr(),
                buf.len(),
                1,
                ClientLevel::Full as u8,
                out.as_mut_ptr(),
            )
        };
        assert_eq!(rc, ffi::X12_PROTO_OK);
        let out = unsafe { out.assume_init() };
        assert_eq!(out.op, 3);
        let cs = unsafe { out.u.create_surface };
        assert_eq!(cs.surface, 0x100);
        assert_eq!(cs.width, 32);
        assert_eq!(cs.height, 48);
        assert_eq!(cs.modifier, 9);
        assert_eq!(cs.format, 0x34325258);
    }

    #[test]
    fn wire_size_constants_match_header() {
        // Keep in sync with safe/x12-proto/include/x12_proto.h and xcb structs.
        assert_eq!(4 + 8, 12); // QueryVersion
        assert_eq!(4 + 68, 72); // CreateSurface
        assert_eq!(4 + 84, 88); // Present
    }
}
