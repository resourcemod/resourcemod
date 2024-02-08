const {metacall} = require('metacall')
process.stdout._orig_write = process.stdout.write;
process.stdout.write = (data) => {
    metacall('log', data)
    process.stdout._orig_write(data);
}