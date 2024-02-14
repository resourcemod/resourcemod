const path = require('path');

module.exports = {
  entry: './src/index.ts', // Замените путь на путь к вашему основному файлу TypeScript
  target: 'node',
  externals: ['node_modules', {
    'metacall': 'commonjs metacall'
  }],
  output: {
    library: {
      type: 'umd',
      export: ['default']
    },
    filename: 'index.js',
    path: path.resolve(__dirname, 'dist'),
    chunkFilename: 'chunks/[name].chunk.js',
  },
  resolve: {
    extensions: ['.ts', '.js'],
    alias: {
      "@src": path.resolve(__dirname, 'src'),
    }
  },
  module: {
    rules: [
      {
        test: /\.ts$/,
        use: 'ts-loader',
        exclude: /node_modules/,
      },
    ],
  },
};