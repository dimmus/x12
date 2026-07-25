//! C ABI for dix integration (ADR-0012).

use crate::frame::DecodeError;
use crate::level::ClientLevel;
use crate::x12_surface::DecodedSurfaceRequest;
use crate::{decode_request, DecodedRequest};

pub const X12_PROTO_OK: i32 = 0;
pub const X12_PROTO_TRUNCATED: i32 = 1;
pub const X12_PROTO_BAD_LENGTH: i32 = 2;
pub const X12_PROTO_MAJOR_MISMATCH: i32 = 3;
pub const X12_PROTO_UNKNOWN_OPCODE: i32 = 4;
pub const X12_PROTO_INVALID_FIELD: i32 = 5;
pub const X12_PROTO_FD_MISMATCH: i32 = 6;
pub const X12_PROTO_DENIED: i32 = 7;
pub const X12_PROTO_BAD_LEVEL: i32 = 8;
pub const X12_PROTO_NULL: i32 = 9;

#[repr(C)]
#[derive(Clone, Copy)]
pub struct X12ProtoQueryVersion {
    pub major_version: u32,
    pub minor_version: u32,
}

#[repr(C)]
#[derive(Clone, Copy)]
pub struct X12ProtoQueryCapabilities {
    pub drawable: u32,
}

#[repr(C)]
#[derive(Clone, Copy)]
pub struct X12ProtoQueryModifiers {
    pub drawable: u32,
    pub format: u32,
}

#[repr(C)]
#[derive(Clone, Copy)]
pub struct X12ProtoCreateSurface {
    pub surface: u32,
    pub drawable: u32,
    pub num_buffers: u8,
    pub depth: u8,
    pub bpp: u8,
    pub pad0: u8,
    pub width: u16,
    pub height: u16,
    pub strides: [u32; 4],
    pub offsets: [u32; 4],
    pub modifier: u64,
    pub format: u32,
}

#[repr(C)]
#[derive(Clone, Copy)]
pub struct X12ProtoDestroySurface {
    pub surface: u32,
}

#[repr(C)]
#[derive(Clone, Copy)]
pub struct X12ProtoImportSyncobj {
    pub syncobj: u32,
    pub drawable: u32,
}

#[repr(C)]
#[derive(Clone, Copy)]
pub struct X12ProtoFreeSyncobj {
    pub syncobj: u32,
}

#[repr(C)]
#[derive(Clone, Copy)]
pub struct X12ProtoAttach {
    pub window: u32,
    pub surface: u32,
}

#[repr(C)]
#[derive(Clone, Copy)]
pub struct X12ProtoDetach {
    pub window: u32,
}

#[repr(C)]
#[derive(Clone, Copy)]
pub struct X12ProtoPresent {
    pub window: u32,
    pub surface: u32,
    pub serial: u32,
    pub x_off: i16,
    pub y_off: i16,
    pub acquire_syncobj: u32,
    pub release_syncobj: u32,
    pub acquire_point: u64,
    pub release_point: u64,
    pub options: u32,
    pub target_msc: u64,
    pub divisor: u64,
    pub remainder: u64,
}

#[repr(C)]
#[derive(Clone, Copy)]
pub struct X12ProtoSelectInput {
    pub eid: u32,
    pub window: u32,
    pub event_mask: u32,
}

#[repr(C)]
pub union X12ProtoSurfacePayload {
    pub query_version: X12ProtoQueryVersion,
    pub query_capabilities: X12ProtoQueryCapabilities,
    pub query_modifiers: X12ProtoQueryModifiers,
    pub create_surface: X12ProtoCreateSurface,
    pub destroy_surface: X12ProtoDestroySurface,
    pub import_syncobj: X12ProtoImportSyncobj,
    pub free_syncobj: X12ProtoFreeSyncobj,
    pub attach: X12ProtoAttach,
    pub detach: X12ProtoDetach,
    pub present: X12ProtoPresent,
    pub select_input: X12ProtoSelectInput,
}

#[repr(C)]
pub struct X12ProtoSurfaceDecoded {
    pub op: u8,
    pub pad: [u8; 3],
    pub u: X12ProtoSurfacePayload,
}

fn map_err(e: DecodeError) -> i32 {
    match e {
        DecodeError::Truncated { .. } => X12_PROTO_TRUNCATED,
        DecodeError::BadLength { .. } => X12_PROTO_BAD_LENGTH,
        DecodeError::MajorMismatch { .. } => X12_PROTO_MAJOR_MISMATCH,
        DecodeError::UnknownOpcode(_) => X12_PROTO_UNKNOWN_OPCODE,
        DecodeError::InvalidField(_) => X12_PROTO_INVALID_FIELD,
        DecodeError::FdCountMismatch { .. } => X12_PROTO_FD_MISMATCH,
        DecodeError::DeniedByLevel { .. } => X12_PROTO_DENIED,
    }
}

fn fill_out(out: &mut X12ProtoSurfaceDecoded, req: &DecodedSurfaceRequest) {
    out.pad = [0; 3];
    match req {
        DecodedSurfaceRequest::QueryVersion {
            major_version,
            minor_version,
        } => {
            out.op = 0;
            out.u.query_version = X12ProtoQueryVersion {
                major_version: *major_version,
                minor_version: *minor_version,
            };
        }
        DecodedSurfaceRequest::QueryCapabilities { drawable } => {
            out.op = 1;
            out.u.query_capabilities = X12ProtoQueryCapabilities {
                drawable: *drawable,
            };
        }
        DecodedSurfaceRequest::QueryModifiers { drawable, format } => {
            out.op = 2;
            out.u.query_modifiers = X12ProtoQueryModifiers {
                drawable: *drawable,
                format: *format,
            };
        }
        DecodedSurfaceRequest::CreateSurface {
            surface,
            drawable,
            num_buffers,
            width,
            height,
            strides,
            offsets,
            depth,
            bpp,
            modifier,
            format,
        } => {
            out.op = 3;
            out.u.create_surface = X12ProtoCreateSurface {
                surface: *surface,
                drawable: *drawable,
                num_buffers: *num_buffers,
                depth: *depth,
                bpp: *bpp,
                pad0: 0,
                width: *width,
                height: *height,
                strides: *strides,
                offsets: *offsets,
                modifier: *modifier,
                format: *format,
            };
        }
        DecodedSurfaceRequest::DestroySurface { surface } => {
            out.op = 4;
            out.u.destroy_surface = X12ProtoDestroySurface {
                surface: *surface,
            };
        }
        DecodedSurfaceRequest::ImportSyncobj { syncobj, drawable } => {
            out.op = 5;
            out.u.import_syncobj = X12ProtoImportSyncobj {
                syncobj: *syncobj,
                drawable: *drawable,
            };
        }
        DecodedSurfaceRequest::FreeSyncobj { syncobj } => {
            out.op = 6;
            out.u.free_syncobj = X12ProtoFreeSyncobj {
                syncobj: *syncobj,
            };
        }
        DecodedSurfaceRequest::Attach { window, surface } => {
            out.op = 7;
            out.u.attach = X12ProtoAttach {
                window: *window,
                surface: *surface,
            };
        }
        DecodedSurfaceRequest::Detach { window } => {
            out.op = 8;
            out.u.detach = X12ProtoDetach { window: *window };
        }
        DecodedSurfaceRequest::Present {
            window,
            surface,
            serial,
            x_off,
            y_off,
            acquire_syncobj,
            release_syncobj,
            acquire_point,
            release_point,
            options,
            target_msc,
            divisor,
            remainder,
        } => {
            out.op = 9;
            out.u.present = X12ProtoPresent {
                window: *window,
                surface: *surface,
                serial: *serial,
                x_off: *x_off,
                y_off: *y_off,
                acquire_syncobj: *acquire_syncobj,
                release_syncobj: *release_syncobj,
                acquire_point: *acquire_point,
                release_point: *release_point,
                options: *options,
                target_msc: *target_msc,
                divisor: *divisor,
                remainder: *remainder,
            };
        }
        DecodedSurfaceRequest::SelectInput {
            eid,
            window,
            event_mask,
        } => {
            out.op = 10;
            out.u.select_input = X12ProtoSelectInput {
                eid: *eid,
                window: *window,
                event_mask: *event_mask,
            };
        }
    }
}

/// # Safety
/// `buf` must be readable for `len` bytes when non-null. `out` may be null.
#[no_mangle]
pub unsafe extern "C" fn x12_proto_decode_surface(
    major_opcode: u8,
    buf: *const u8,
    len: usize,
    fds_attached: u32,
    level: u8,
    out: *mut X12ProtoSurfaceDecoded,
) -> i32 {
    if buf.is_null() || len == 0 {
        return X12_PROTO_NULL;
    }
    let Some(level) = ClientLevel::from_u8(level) else {
        return X12_PROTO_BAD_LEVEL;
    };
    let slice = unsafe { core::slice::from_raw_parts(buf, len) };
    match decode_request(major_opcode, slice, fds_attached, level) {
        Ok(DecodedRequest::X12Surface(req)) => {
            if !out.is_null() {
                // SAFETY: non-null out provided by caller.
                unsafe {
                    fill_out(&mut *out, &req);
                }
            }
            X12_PROTO_OK
        }
        Err(e) => map_err(e),
    }
}

#[no_mangle]
pub extern "C" fn x12_proto_version() -> *const core::ffi::c_char {
    c"0.2.0-prep".as_ptr()
}
