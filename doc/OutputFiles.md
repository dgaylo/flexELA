# Output Files {#OutputFiles}

## Volume Tracking Matrix {#volumetrackingmatrix}

Contains the (non-normalized) volume tracking matrix (VTM) \f$\mathbf{Q}^{(n-1\rightarrow n)}\f$ and is output by calling \ref ELA_OutputWriteVTM().
The VTM is stored in CSR matrix format. The first entry is the row count (RC) and the second is the number of non-zero entries (NNZ).

| Description | Type |
|--|--|
| RC | `uint32_t` |
| NNZ | `uint32_t` |
| ROW_INDEX | `uint32_t[RC]` |
| COLUMN_INDEX | `uint32_t[NNZ]` |
| VALUES | `double[NNZ]` |

### Naming

This file is named `afwd_[n].bin`, where `n` is the snapshot index and is zero-padded to 6 digits.

### Notes

- The column count (CC) is not given by this file. If required, obtain from [`timelog.bin`](#timelogbin).
- The zero entry that formally starts `ROW_INDEX` is omitted.
- To verify the file, check that `ROW_INDEX(end)==NNZ`.

## Volume Vector {#volumevector}

Contains the volume vector \f$\mathbf{v}^{n}\f$ (see @cite Gaylo2022, Eq. 8) and is output by calling \ref ELA_OutputWriteV.
The first entry gives \f$M^{n}\f$, the row count (RC). This is followed values corresponding to \f$v_l^{n}\f$ for \f$l\in 1\dots M^{n}\f$, which may contain zeros.

| Description | Type |
|--|--|
| RC | `uint32_t` |
| VALUES | `double[RC]` |

### Naming

This file is named `v_[n].bin`, where `n` is the snapshot index and is zero-padded to 6 digits.

## timelog.bin {#timelogbin}

This file is appended to each time \ref ELA_OutputWriteVTM() is called and records \f$ n \f$, the row count (RC) \f$ M^{n} \f$, and the time \f$ t^{n} \f$.

| Description | Type |
|--|--|
| Index, \f$ n \f$ | `uint32_t` |
| RC, \f$ M^{n} \f$| `uint32_t` |
| Time, \f$ t^{n} \f$ |  `double` |

## tracking.log {#trackinglog}

This file's purpose is to allow one to monitor the volume conservativeness of the tracking data. When \ref ELA_OutputLog() is called, the following data is appended:

| Time | Max Label | Max Source Entry | Min Source Entry | Volume Error (abs) | Volume Error (rel) | Max NNZ |
|--|--|--|--|--|--|--|
| $$t^n$$ | $$M^{n}$$ | $$1-\max\left[\mathbf{s}^{n}_{ijk}\right]$$ | $$\min\left[\\{\mathbf{s}^{n}_{ijk} : \mathbf{s}^{n}\_{ijk}\ne 0\\}\right]$$ | $$\sum_{ijk}\left[\Omega_{ijk} \sum_l {(s^{n}\_l)}\_{ijk} - \Omega_{ijk} (1-f_{ijk})\right]$$ | $$\frac{\text{Volume Error (abs)}}{\sum_{ijk}\left[\Omega_{ijk} (1-f_{ijk})\right]}$$ | $$\max\\{\text{nnz}[\mathbf{s}^{n}]\\}$$|

[ela-paper]: https://doi.org/10.1016/j.jcp.2022.111560