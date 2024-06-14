/*
Settings for binary tracking data output files
----------------------------------------------
    Name Format: {TRACKING_MATRIX_FILENAME}[{t_num}].{TRACKING_MATRIX_FILENAME_EXT}
    Name Format: {VOLUME_VECTOR_FILENAME}[{t_num}].{VOLUME_VECTOR_FILENAME_EXT}
    Name Format: {TIMELOG_FILENAME}.{TIMELOG_FILENAME_EXT}

Settings for ascii tracking data output files
---------------------------------------------
    Name Format: {TRACKING_LOG_FILENAME}
*/
#ifndef T_NUM_DIGITS
#define T_NUM_DIGITS 6
#endif

#ifndef TRACKING_MATRIX_FILENAME
#define TRACKING_MATRIX_FILENAME "afwd_"
#endif

#ifndef TRACKING_MATRIX_FILENAME_EXT
#define TRACKING_MATRIX_FILENAME_EXT "bin"
#endif

#ifndef VOLUME_VECTOR_FILENAME
#define VOLUME_VECTOR_FILENAME "v_"
#endif

#ifndef VOLUME_VECTOR_FILENAME_EXT
#define VOLUME_VECTOR_FILENAME_EXT "bin"
#endif

#ifndef TIMELOG_FILENAME
#define TIMELOG_FILENAME "timelog"
#endif

#ifndef TIMELOG_FILENAME_EXT
#define TIMELOG_FILENAME_EXT "bin"
#endif