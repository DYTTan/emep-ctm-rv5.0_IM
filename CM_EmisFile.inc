character(len=4), save, dimension(NEMIS_File), public :: EMIS_File = (/ &
  "sox " &
, "nox " &
, "co  " &
, "voc " &
, "nh3 " &
, "pm25" &
, "pmco" &
, "hcl " & !DYTT 25/02/26 added this line
, "pcl " & !DYTT 25/02/26 added this line
/)
