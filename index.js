const express = require('express');
const app = express();
const port = process.env.PORT || 3000;

var options = {
  dotfiles: 'ignore',
  etag: false,
  extensions: ['htm', 'html','css','js','ico','jpg','jpeg','png','svg', 'mp4'],
  index: ['index.html'],
  maxAge: '1m',
  redirect: false
};
app.use(express.static('public', options));

const execSync = require('child_process').execSync;
const fs = require("fs");
const path = require("path");
const multer = require("multer");

const handleError = (err, res) => {
  res
    .status(500)
    .contentType("text/plain")
    .end("Oops! Something went wrong!");
};

const upload = multer({
  dest: "./uploads"
  //setting limits: https://github.com/expressjs/multer#limits
});

function del_mp4(path) {
  execSync('rm ' + path, { encoding: 'utf-8' });
}

app.post(
  "/upload",
  upload.single("file" /* name attribute of <file> element in form */),
  (req, res) => {
    const tempPath = req.file.path;
    const targetPath = path.join(__dirname, "public/uploads/image.png");

    //console.log(req);
    //console.log(req.body["r-radius"]);

    const r_center  = req.body["r-center"];
    const r_radius  = req.body["r-radius"];
    const c_center  = req.body["c-center"];
    const c_radius  = req.body["c-radius"];
    const scale     = req.body["scale"];
    const no_frames = req.body["no-frames"];

    const ext = path.extname(req.file.originalname).toLowerCase();

    if(ext == ".jpeg" || ext == ".jpg" || ext === ".png" || ext == ".svg") {
      //const output = execSync('./granimate ' + req.file.filename + ' ' + r_center + ' ' + r_radius + ' ' + c_center + ' ' + c_radius + ' ' + scale + ' ' + no_frames, { encoding: 'utf-8' });

      //deletes mp4 15 mins later
      //setTimeout(function(){del_mp4('public/mp4/' + req.file.filename + '.mp4')}, 60*1000);
      setTimeout(function(){del_mp4('public/mp4/' + req.file.filename + '.mp4')}, 15*60*1000);

      res.send('{"path": "../mp4/' + req.file.filename + '.mp4"}');
      res.status(200).end();

      /*fs.rename(tempPath, targetPath, err => {
        if(err) 
          return handleError(err, res);
      });*/
    } else {
      fs.unlink(tempPath, err => {
        if(err) 
          return handleError(err, res);

        res.status(403).contentType("text/plain").end("Only .png files are allowed!");
      });
    }
  }
);

app.listen(port, () => {
  console.log(`Example app served at http://localhost:${port}/`);
});