class Color {
    red
    green
    blue
    alpha
    constructor(red, green, blue, alpha) {
        this.red = red
        this.green = green
        this.blue = blue
        this.alpha = alpha

        this.toString = (separator) => {
            return this.red + separator + this.green + separator + this.blue + separator + this.alpha
        }
    }
}

module.exports = {
    Color
}