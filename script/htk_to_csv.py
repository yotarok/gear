import sys
import struct

def read_htk(path):
    ret = []
    f = open(path)
    (n_samples, samp_period, samp_size, parm_kind
     ) = struct.unpack('>IIHH', f.read(12))
    dims = samp_size / 4
    for i in range(n_samples):
        row = list(struct.unpack('>' + 'f' * dims, f.read(4 * dims)))

        ret.append(row)
    return ret

for row in read_htk(sys.argv[1]):
    print(','.join(str(e) for e in row))

