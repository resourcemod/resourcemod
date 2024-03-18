export class Color {
    red: number;
    green: number;
    blue: number;
    alpha: number;

    constructor(red: number, green: number, blue: number, alpha: number) {
        this.red = red
        this.green = green
        this.blue = blue
        this.alpha = alpha
    }

    toString(separator: string = ', ') {
        return `${this.red}${separator}${this.green}${separator}${this.blue}${separator}${this.alpha}`
    }
}

export const _CreateColor = (red: number, green: number, blue: number, alpha: number) => {
    return new Color(red, green, blue, alpha);
}