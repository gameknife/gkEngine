var path = require('path');
module.exports = {
    entry: {
        main:'./editor/main.js',
        renderer:'./editor/editor-main.js'
    },
    output: {
        path: path.resolve(__dirname, 'build'),
        filename: '[name].js'
    },
    target: 'electron',
    context: __dirname,
    devtool: 'cheap-module-eval-source-map',
    node: {
        __filename: false,
        __dirname: false
    },
    module: {
        rules: [{
            test: /.node$/,
            use: 'node-loader'
        }
          ],
        loaders: [{
            exclude: /(node_modules|bower_components)/,
            loader: 'babel-loader', // 'babel-loader' is also a legal name to reference
            query: {
                presets: ['es2015']
            }
        }]
    },
    devServer: {
        contentBase: './build'
    }


};
