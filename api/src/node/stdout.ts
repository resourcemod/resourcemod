// @ts-ignore
import { metacall } from "metacall"

process.stdout._orig_write = process.stdout.write;
// @ts-expect-error
process.stdout.write = (data) => {
    metacall('log', data)
    process.stdout._orig_write(data);
}

process.stderr._orig_write = process.stderr.write;
// @ts-expect-error
process.stderr.write = (data) => {
    metacall('err', data)
    process.stderr._orig_write(data);
}