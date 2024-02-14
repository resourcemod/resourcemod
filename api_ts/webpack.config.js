const path = require('path');
const fs = require('fs');

const entryPaths = {
  'index': path.resolve(__dirname, './src/index.ts'),
};
fs.readdirSync('./src/node').forEach(file => {
  if (!file.endsWith('.ts')) return;
  // entryPaths.push({
  //   import: path.resolve(__dirname, `./src/node/${file}`),
  //   filename: `node/${file.replace('.ts', '.js')}`
  // });
  entryPaths[`node/${file.replace('.ts', '')}`] = {
    import: path.resolve(__dirname, `./src/node/${file}`),
    filename: `node/${file.replace('.ts', '.js')}`
  };
})

module.exports = {
  entry: entryPaths,
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