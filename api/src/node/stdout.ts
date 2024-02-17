// @ts-expect-error Could not find a declaration file for module 'metacall'
import { metacall } from "metacall"

process.stdout._orig_write = process.stdout.write;
process.stdout.write = (data) => {
    metacall("log", data)
    return process.stdout._orig_write(data);
}

process.stderr._orig_write = process.stderr.write;
process.stderr.write = (data) => {
    metacall("err", data)
    return process.stderr._orig_write(data);
}