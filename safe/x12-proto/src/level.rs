//! Hierarchical client levels (ADR-0003 / X12-LEVEL) applied at decode time.

use crate::frame::DecodeError;
use crate::x12_surface::{DecodedSurfaceRequest, SurfaceOpcode};

/// Client trust level. Ordinal: Sandbox < User < Full.
#[repr(u8)]
#[derive(Debug, Clone, Copy, PartialEq, Eq, PartialOrd, Ord)]
pub enum ClientLevel {
    Sandbox = 0,
    User = 1,
    Full = 2,
}

impl ClientLevel {
    pub fn as_str(self) -> &'static str {
        match self {
            Self::Sandbox => "sandbox",
            Self::User => "user",
            Self::Full => "full",
        }
    }

    pub fn from_u8(v: u8) -> Option<Self> {
        match v {
            0 => Some(Self::Sandbox),
            1 => Some(Self::User),
            2 => Some(Self::Full),
            _ => None,
        }
    }
}

/// Spike policy for X12-SURFACE (refine with full request matrix later).
///
/// - **sandbox:** QueryVersion / QueryCapabilities / QueryModifiers only
/// - **user / full:** all surface opcodes
pub fn authorize_surface(
    req: &DecodedSurfaceRequest,
    level: ClientLevel,
) -> Result<(), DecodeError> {
    let op = req.opcode();
    let allowed = match level {
        ClientLevel::Sandbox => matches!(
            op,
            SurfaceOpcode::QueryVersion
                | SurfaceOpcode::QueryCapabilities
                | SurfaceOpcode::QueryModifiers
        ),
        ClientLevel::User | ClientLevel::Full => true,
    };
    if allowed {
        Ok(())
    } else {
        Err(DecodeError::DeniedByLevel {
            level: level.as_str(),
            op: op.as_str(),
        })
    }
}
