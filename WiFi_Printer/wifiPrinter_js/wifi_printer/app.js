/**
 * @File: app.js
 *
 */
var connect = require('connect');
var users = require('./users');
var fs = require('fs');

var server = connect(
    connect.logger('dev'),
    connect.bodyParser(),
    connect.cookieParser(),
    connect.session({ secret: 'my app secret' })
);

server.use(function (req, res, next) {
    if ('/' == req.url && req.session.logged_in) {
        res.writeHead(200, { 'Content-Type': 'text/html' });
        fs.createReadStream(__dirname + '/html/index.html').pipe(res);              
    } else {
        next();
    }
});

server.use(function (req, res, next) {
    if ('/' == req.url && 'GET' == req.method) {
        res.writeHead(200, { 'Content-Type': 'text/html' });
        fs.createReadStream(__dirname + '/html/login.html').pipe(res);
    } else {
        next();
    }
});

server.use(function (req, res, next) {
    if ('/login' == req.url && 'POST' == req.method) {
        if (!users[req.body.user] || req.body.password != users[req.body.user].password) {
            res.writeHead(200);
            fs.createReadStream(__dirname + '/html/logerror.html').pipe(res);
        } else {
            req.session.logged_in = true;
            req.session.name = users[req.body.user].name;
            res.writeHead(200, { 'Content-Type': 'text/html' });
            fs.createReadStream(__dirname + '/html/index.html').pipe(res);
        }
    } else {
        next();
    }
});

server.use(function (req, res, next) {
    if ('/logout' == req.url) {
        req.session.logged_in = false;
        res.writeHead(200, { 'Content-Type': 'text/html' });
        fs.createReadStream(__dirname + '/html/logout.html').pipe(res);
    } else {
        next();
    }
});

server.use(function (req, res, next) {
    if ('/print' == req.url) {
        var message;
        console.log( message=req.body.document);
        // Parameter path in writeFile(message, path) is a absolute path，
        // once you keep this project to other place, remember
        // to change any read write path in this project and
        // the arduino one.
        require('./file_write').writeFile(message, "/root/wifi_printer/files/message.txt");
        require('./file_write').writeFile('1', "/root/wifi_printer/files/printer_status.txt");
    }
});

server.listen(3000);
