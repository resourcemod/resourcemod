const { metacall } = require('metacall')
process.stdout._orig_write = process.stdout.write;
process.stdout.write = (data) => {
    metacall('log', data)
    process.stdout._orig_write(data);
}

process.stderr._orig_write = process.stderr.write;
process.stderr.write = (data) => {
    metacall('err', data)
    process.stderr._orig_write(data);
}