subdir('amdgpu')
# subdir('armsoc')  # TODO: implement, conflicting with amdgpu
subdir('ati')
subdir('dummy')
subdir('fbdev')
subdir('intel')     # TODO: finish with acceleration support (sna, uxa)
subdir('nouveau')
subdir('qxl')       # TODO: implement spice
# subdir('vbox')    # TODO: need "VboxVideo.h" (need dep_vbox?)
subdir('vesa')
# Needs Mesa xatracker (Gallium XA). Absent on Mesa ≥25.2 / current Arch.
if dep_xa.found()
  subdir('vmware')
else
  message('skipping video/vmware: xatracker (libxatracker) not found')
endif