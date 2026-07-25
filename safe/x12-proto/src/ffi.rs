//! C ABI for future server integration (step 7+).

use crate::frame::DecodeError;
use crate::level::ClientLevel;
use crate::{decode_request, DecodedRequest};

/// C-visible status codes (stable for the spike).
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

/// Decode + validate one X12-SURFACE request.
///
/// On success, writes the minor opcode into `out_minor` (if non-NULL) and
/// returns [`X12_PROTO_OK`]. Does not take ownership of `buf`.
///
/// # Safety
/// `buf` must be readable for `len` bytes when non-null. `out_minor` may be null.
#[no_mangle]
pub unsafe extern "C" fn x12_proto_decode_surface(
    major_opcode: u8,
    buf: *const u8,
    len: usize,
    fds_attached: u32,
    level: u8,
    out_minor: *mut u8,
) -> i32 {
    if buf.is_null() || len == 0 {
        return X12_PROTO_NULL;
    }
    let Some(level) = ClientLevel::from_u8(level) else {
        return X12_PROTO_BAD_LEVEL;
    };
    // SAFETY: caller guarantees `buf` is valid for `len` bytes.
    let slice = unsafe { core::slice::from_raw_parts(buf, len) };
    match decode_request(major_opcode, slice, fds_attached, level) {
        Ok(DecodedRequest::X12Surface(req)) => {
            if !out_minor.is_null() {
                // SAFETY: non-null pointer provided by caller.
                unsafe {
                    *out_minor = req.opcode() as u8;
                }
            }
            X12_PROTO_OK
        }
        Err(e) => map_err(e),
    }
}

/// Library version string for diagnostics.
#[no_mangle]
pub extern "C" fn x12_proto_version() -> *const core::ffi::c_char {
    c"0.1.0-spike".as_ptr()
}
