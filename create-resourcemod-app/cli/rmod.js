#!/usr/bin/env node

const https = require('node:https');
const fs = require('node:fs');
const {execSync} = require('node:child_process')
const {platform} = require('node:process');

if (platform !== 'win32') {
    console.error('ResourceMod only runs on Windows.');
    return false;
}
if (!fs.existsSync('./metamod')) {
    console.error('MetaMod is not installed.');
    console.error('Please install MetaMod first: https://wiki.alliedmods.net/Installing_metamod:source');
    return false;
}

const node_loader = fs.createWriteStream("./metamod/.node-loader.vdf");
https.get("https://s3.eu-central-1.amazonaws.com/saido.static/vdfs/.node-loader.vdf", function (response) {
    response.pipe(node_loader);
    node_loader.on("finish", () => {
        node_loader.close();
        console.log("[NodeLoader] Download Completed!");
    });
});

const resourcemod = fs.createWriteStream("./metamod/resourcemod.vdf");
https.get("https://s3.eu-central-1.amazonaws.com/saido.static/vdfs/resourcemod.vdf", function (response) {
    response.pipe(resourcemod);
    resourcemod.on("finish", () => {
        resourcemod.close();
        console.log("[ResourceMod] Download Completed!");
    });
});

try {
    const repository = 'https://github.com/resourcemod/package-template.git'
    const tag = 'v1.0.32'
    execSync(`git clone --depth 1 --branch ${tag} ${repository} resourcemod`);
} catch (e) {
    console.error("Failed to clone resourcemod package.");
    return false;
}

try {
    execSync(`cd resourcemod && npm install`);
} catch (e) {
    console.error("Failed to install resourcemod package.");
    return false;
}

try {
    fs.rmdirSync("./resourcemod/.git", {
        recursive: true
    })
} catch (e) {
    console.error("Failed to remove .git dependencies.")
    return false;
}