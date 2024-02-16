export class Color {
    constructor(
        public red: number,
        public green: number,
        public blue: number,
        public alpha: number
    ) {
    }

    toString(separator: string) {
        return `${this.red}${separator}${this.green}${separator}${this.blue}${separator}${this.alpha}`
    }
}