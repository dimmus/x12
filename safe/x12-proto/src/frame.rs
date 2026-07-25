//! X11 request framing (major / minor / length).

use core::fmt;

/// Errors from framing or semantic validation.
#[derive(Debug, Clone, PartialEq, Eq)]
pub enum DecodeError {
    Truncated { need: usize, got: usize },
    BadLength { units: u16, bytes: usize },
    MajorMismatch { expected: u8, got: u8 },
    UnknownOpcode(u8),
    InvalidField(&'static str),
    FdCountMismatch { expected: u32, got: u32 },
    DeniedByLevel { level: &'static str, op: &'static str },
}

impl fmt::Display for DecodeError {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self {
            Self::Truncated { need, got } => {
                write!(f, "truncated request: need {need} bytes, got {got}")
            }
            Self::BadLength { units, bytes } => {
                write!(f, "length {units} units incompatible with {bytes} bytes")
            }
            Self::MajorMismatch { expected, got } => {
                write!(f, "major opcode mismatch: expected {expected}, got {got}")
            }
            Self::UnknownOpcode(op) => write!(f, "unknown minor opcode {op}"),
            Self::InvalidField(name) => write!(f, "invalid field: {name}"),
            Self::FdCountMismatch { expected, got } => {
                write!(f, "fd count mismatch: expected {expected}, got {got}")
            }
            Self::DeniedByLevel { level, op } => {
                write!(f, "client level {level} denied opcode {op}")
            }
        }
    }
}

impl std::error::Error for DecodeError {}

/// Parsed X11 request header (4 bytes).
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub struct RequestHeader {
    pub major_opcode: u8,
    pub minor_opcode: u8,
    /// Length in 4-byte units, including this header.
    pub length_units: u16,
}

impl RequestHeader {
    pub fn parse(buf: &[u8]) -> Result<Self, DecodeError> {
        if buf.len() < 4 {
            return Err(DecodeError::Truncated {
                need: 4,
                got: buf.len(),
            });
        }
        let length_units = u16::from_le_bytes([buf[2], buf[3]]);
        if length_units == 0 {
            return Err(DecodeError::BadLength {
                units: 0,
                bytes: buf.len(),
            });
        }
        let need = (length_units as usize).saturating_mul(4);
        if buf.len() < need {
            return Err(DecodeError::Truncated {
                need,
                got: buf.len(),
            });
        }
        // Reject trailing garbage beyond the declared length for this spike.
        if buf.len() != need {
            return Err(DecodeError::BadLength {
                units: length_units,
                bytes: buf.len(),
            });
        }
        Ok(Self {
            major_opcode: buf[0],
            minor_opcode: buf[1],
            length_units,
        })
    }

    pub fn body<'a>(&self, buf: &'a [u8]) -> Result<&'a [u8], DecodeError> {
        let need = (self.length_units as usize) * 4;
        if buf.len() < need {
            return Err(DecodeError::Truncated {
                need,
                got: buf.len(),
            });
        }
        Ok(&buf[4..need])
    }
}

pub(crate) fn read_u16(buf: &[u8], off: usize) -> Result<u16, DecodeError> {
    let end = off + 2;
    if buf.len() < end {
        return Err(DecodeError::Truncated {
            need: end,
            got: buf.len(),
        });
    }
    Ok(u16::from_le_bytes([buf[off], buf[off + 1]]))
}

pub(crate) fn read_u32(buf: &[u8], off: usize) -> Result<u32, DecodeError> {
    let end = off + 4;
    if buf.len() < end {
        return Err(DecodeError::Truncated {
            need: end,
            got: buf.len(),
        });
    }
    Ok(u32::from_le_bytes(
        buf[off..end].try_into().expect("length checked"),
    ))
}

pub(crate) fn read_u64(buf: &[u8], off: usize) -> Result<u64, DecodeError> {
    let end = off + 8;
    if buf.len() < end {
        return Err(DecodeError::Truncated {
            need: end,
            got: buf.len(),
        });
    }
    Ok(u64::from_le_bytes(
        buf[off..end].try_into().expect("length checked"),
    ))
}

pub(crate) fn expect_len(buf: &[u8], need: usize) -> Result<(), DecodeError> {
    if buf.len() != need {
        return Err(DecodeError::InvalidField("request body length"));
    }
    Ok(())
}
