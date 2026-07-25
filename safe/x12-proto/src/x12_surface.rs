//! Decode/validate X12-SURFACE requests (`proto/xcb/src/x12_surface.xml`).

use crate::frame::{expect_len, read_u16, read_u32, read_u64, DecodeError};

/// Minor opcodes (must match XML).
#[repr(u8)]
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum SurfaceOpcode {
    QueryVersion = 0,
    QueryCapabilities = 1,
    QueryModifiers = 2,
    CreateSurface = 3,
    DestroySurface = 4,
    ImportSyncobj = 5,
    FreeSyncobj = 6,
    Attach = 7,
    Detach = 8,
    Present = 9,
    SelectInput = 10,
}

impl SurfaceOpcode {
    pub fn from_u8(v: u8) -> Result<Self, DecodeError> {
        Ok(match v {
            0 => Self::QueryVersion,
            1 => Self::QueryCapabilities,
            2 => Self::QueryModifiers,
            3 => Self::CreateSurface,
            4 => Self::DestroySurface,
            5 => Self::ImportSyncobj,
            6 => Self::FreeSyncobj,
            7 => Self::Attach,
            8 => Self::Detach,
            9 => Self::Present,
            10 => Self::SelectInput,
            other => return Err(DecodeError::UnknownOpcode(other)),
        })
    }

    pub fn as_str(self) -> &'static str {
        match self {
            Self::QueryVersion => "QueryVersion",
            Self::QueryCapabilities => "QueryCapabilities",
            Self::QueryModifiers => "QueryModifiers",
            Self::CreateSurface => "CreateSurface",
            Self::DestroySurface => "DestroySurface",
            Self::ImportSyncobj => "ImportSyncobj",
            Self::FreeSyncobj => "FreeSyncobj",
            Self::Attach => "Attach",
            Self::Detach => "Detach",
            Self::Present => "Present",
            Self::SelectInput => "SelectInput",
        }
    }
}

/// Validated X12-SURFACE request.
#[derive(Debug, Clone, PartialEq, Eq)]
pub enum DecodedSurfaceRequest {
    QueryVersion {
        major_version: u32,
        minor_version: u32,
    },
    QueryCapabilities {
        drawable: u32,
    },
    QueryModifiers {
        drawable: u32,
        format: u32,
    },
    CreateSurface {
        surface: u32,
        drawable: u32,
        num_buffers: u8,
        width: u16,
        height: u16,
        strides: [u32; 4],
        offsets: [u32; 4],
        depth: u8,
        bpp: u8,
        modifier: u64,
        format: u32,
    },
    DestroySurface {
        surface: u32,
    },
    ImportSyncobj {
        syncobj: u32,
        drawable: u32,
    },
    FreeSyncobj {
        syncobj: u32,
    },
    Attach {
        window: u32,
        surface: u32,
    },
    Detach {
        window: u32,
    },
    Present {
        window: u32,
        surface: u32,
        serial: u32,
        x_off: i16,
        y_off: i16,
        acquire_syncobj: u32,
        release_syncobj: u32,
        acquire_point: u64,
        release_point: u64,
        options: u32,
        target_msc: u64,
        divisor: u64,
        remainder: u64,
    },
    SelectInput {
        eid: u32,
        window: u32,
        event_mask: u32,
    },
}

impl DecodedSurfaceRequest {
    pub fn opcode(&self) -> SurfaceOpcode {
        match self {
            Self::QueryVersion { .. } => SurfaceOpcode::QueryVersion,
            Self::QueryCapabilities { .. } => SurfaceOpcode::QueryCapabilities,
            Self::QueryModifiers { .. } => SurfaceOpcode::QueryModifiers,
            Self::CreateSurface { .. } => SurfaceOpcode::CreateSurface,
            Self::DestroySurface { .. } => SurfaceOpcode::DestroySurface,
            Self::ImportSyncobj { .. } => SurfaceOpcode::ImportSyncobj,
            Self::FreeSyncobj { .. } => SurfaceOpcode::FreeSyncobj,
            Self::Attach { .. } => SurfaceOpcode::Attach,
            Self::Detach { .. } => SurfaceOpcode::Detach,
            Self::Present { .. } => SurfaceOpcode::Present,
            Self::SelectInput { .. } => SurfaceOpcode::SelectInput,
        }
    }
}

pub fn decode(
    minor: u8,
    body: &[u8],
    fds_attached: u32,
) -> Result<DecodedSurfaceRequest, DecodeError> {
    let op = SurfaceOpcode::from_u8(minor)?;
    match op {
        SurfaceOpcode::QueryVersion => {
            expect_len(body, 8)?;
            Ok(DecodedSurfaceRequest::QueryVersion {
                major_version: read_u32(body, 0)?,
                minor_version: read_u32(body, 4)?,
            })
        }
        SurfaceOpcode::QueryCapabilities => {
            expect_len(body, 4)?;
            Ok(DecodedSurfaceRequest::QueryCapabilities {
                drawable: read_u32(body, 0)?,
            })
        }
        SurfaceOpcode::QueryModifiers => {
            expect_len(body, 8)?;
            Ok(DecodedSurfaceRequest::QueryModifiers {
                drawable: read_u32(body, 0)?,
                format: read_u32(body, 4)?,
            })
        }
        SurfaceOpcode::CreateSurface => decode_create_surface(body, fds_attached),
        SurfaceOpcode::DestroySurface => {
            expect_len(body, 4)?;
            Ok(DecodedSurfaceRequest::DestroySurface {
                surface: read_u32(body, 0)?,
            })
        }
        SurfaceOpcode::ImportSyncobj => {
            expect_len(body, 8)?;
            if fds_attached != 1 {
                return Err(DecodeError::FdCountMismatch {
                    expected: 1,
                    got: fds_attached,
                });
            }
            Ok(DecodedSurfaceRequest::ImportSyncobj {
                syncobj: read_u32(body, 0)?,
                drawable: read_u32(body, 4)?,
            })
        }
        SurfaceOpcode::FreeSyncobj => {
            expect_len(body, 4)?;
            expect_no_fds(fds_attached)?;
            Ok(DecodedSurfaceRequest::FreeSyncobj {
                syncobj: read_u32(body, 0)?,
            })
        }
        SurfaceOpcode::Attach => {
            expect_len(body, 8)?;
            expect_no_fds(fds_attached)?;
            Ok(DecodedSurfaceRequest::Attach {
                window: read_u32(body, 0)?,
                surface: read_u32(body, 4)?,
            })
        }
        SurfaceOpcode::Detach => {
            expect_len(body, 4)?;
            expect_no_fds(fds_attached)?;
            Ok(DecodedSurfaceRequest::Detach {
                window: read_u32(body, 0)?,
            })
        }
        SurfaceOpcode::Present => decode_present(body, fds_attached),
        SurfaceOpcode::SelectInput => {
            expect_len(body, 12)?;
            expect_no_fds(fds_attached)?;
            let mask = read_u32(body, 8)?;
            // EventMask bits 0..2 only in v1.0
            if mask & !0b111 != 0 {
                return Err(DecodeError::InvalidField("event_mask"));
            }
            Ok(DecodedSurfaceRequest::SelectInput {
                eid: read_u32(body, 0)?,
                window: read_u32(body, 4)?,
                event_mask: mask,
            })
        }
    }
}

fn expect_no_fds(fds_attached: u32) -> Result<(), DecodeError> {
    if fds_attached != 0 {
        return Err(DecodeError::FdCountMismatch {
            expected: 0,
            got: fds_attached,
        });
    }
    Ok(())
}

fn decode_create_surface(
    body: &[u8],
    fds_attached: u32,
) -> Result<DecodedSurfaceRequest, DecodeError> {
    // Matches x12_surface.xml CreateSurface body (no header).
    expect_len(body, 68)?;
    let surface = read_u32(body, 0)?;
    let drawable = read_u32(body, 4)?;
    let num_buffers = body[8];
    if !(1..=4).contains(&num_buffers) {
        return Err(DecodeError::InvalidField("num_buffers"));
    }
    if body[9] != 0 || body[10] != 0 || body[11] != 0 {
        return Err(DecodeError::InvalidField("num_buffers_pad"));
    }
    if fds_attached != u32::from(num_buffers) {
        return Err(DecodeError::FdCountMismatch {
            expected: u32::from(num_buffers),
            got: fds_attached,
        });
    }
    let width = read_u16(body, 12)?;
    let height = read_u16(body, 14)?;
    if width == 0 || height == 0 {
        return Err(DecodeError::InvalidField("geometry"));
    }
    let mut strides = [0u32; 4];
    let mut offsets = [0u32; 4];
    for i in 0..4 {
        strides[i] = read_u32(body, 16 + i * 8)?;
        offsets[i] = read_u32(body, 20 + i * 8)?;
    }
    // Unused planes must be zero.
    for i in (num_buffers as usize)..4 {
        if strides[i] != 0 || offsets[i] != 0 {
            return Err(DecodeError::InvalidField("unused_plane"));
        }
    }
    let depth = body[48];
    let bpp = body[49];
    if body[50] != 0 || body[51] != 0 {
        return Err(DecodeError::InvalidField("depth_bpp_pad"));
    }
    if depth == 0 || bpp == 0 {
        return Err(DecodeError::InvalidField("depth_bpp"));
    }
    let modifier = read_u64(body, 52)?;
    let format = read_u32(body, 60)?;
    if format == 0 {
        return Err(DecodeError::InvalidField("format"));
    }
    if read_u32(body, 64)? != 0 {
        return Err(DecodeError::InvalidField("format_pad"));
    }
    if surface == 0 || drawable == 0 {
        return Err(DecodeError::InvalidField("xid"));
    }
    Ok(DecodedSurfaceRequest::CreateSurface {
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
    })
}

fn decode_present(body: &[u8], fds_attached: u32) -> Result<DecodedSurfaceRequest, DecodeError> {
    expect_len(body, 84)?;
    expect_no_fds(fds_attached)?;
    let window = read_u32(body, 0)?;
    let surface = read_u32(body, 4)?;
    let serial = read_u32(body, 8)?;
    // pad 8
    for b in &body[12..20] {
        if *b != 0 {
            return Err(DecodeError::InvalidField("present_pad0"));
        }
    }
    let x_off = read_u16(body, 20)? as i16;
    let y_off = read_u16(body, 22)? as i16;
    for b in &body[24..28] {
        if *b != 0 {
            return Err(DecodeError::InvalidField("present_pad1"));
        }
    }
    let acquire_syncobj = read_u32(body, 28)?;
    let release_syncobj = read_u32(body, 32)?;
    let acquire_point = read_u64(body, 36)?;
    let release_point = read_u64(body, 44)?;
    let options = read_u32(body, 52)?;
    // PresentOption bits 0..2
    if options & !0b111 != 0 {
        return Err(DecodeError::InvalidField("options"));
    }
    for b in &body[56..60] {
        if *b != 0 {
            return Err(DecodeError::InvalidField("present_pad2"));
        }
    }
    let target_msc = read_u64(body, 60)?;
    let divisor = read_u64(body, 68)?;
    let remainder = read_u64(body, 76)?;
    if window == 0 || surface == 0 {
        return Err(DecodeError::InvalidField("xid"));
    }
    Ok(DecodedSurfaceRequest::Present {
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
    })
}
