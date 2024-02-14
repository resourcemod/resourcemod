#!/usr/bin/env node

const https = require("node:https");
const fs = require("node:fs");
const { execSync } = require("node:child_process");
const { platform } = require("node:process");
const readline = require("node:readline");

if (platform !== "win32") {
  console.error("ResourceMod only runs on Windows.");
  return false;
}

if (!fs.existsSync("./metamod")) {
  console.error("MetaMod is not installed.");
  console.error(
    "Please install MetaMod first: https://wiki.alliedmods.net/Installing_metamod:source"
  );
  return false;
}

const rl = readline.createInterface({
  input: process.stdin,
  output: process.stdout,
});

rl.question(
  `Which language do you want to use? 
    Enter 'javascript' or 'typescript'.
    Press enter to use the default (javascript): `,
  (answer) => {
    let repository;
    if (answer === "" || answer === "javascript") {
      var tag = "v1.0.10";
      repository = "https://github.com/resourcemod/package-template.git";
    } else if (answer === "typescript") {
      // var tag = 'v1.0.10';
      repository = "https://github.com/resourcemod/package-template-typescript.git";
    } else {
      console.error(
        "Invalid option. Please choose either javascript or typescript."
      );
      rl.close();
      return false;
    }
    // rest of the code...

    const node_loader = fs.createWriteStream("./metamod/.node-loader.vdf");
    https.get(
      "https://s3.eu-central-1.amazonaws.com/saido.static/vdfs/.node-loader.vdf",
      function (response) {
        response.pipe(node_loader);
        node_loader.on("finish", () => {
          node_loader.close();
          console.log("[NodeLoader] Download Completed!");
        });
      }
    );

    const resourcemod = fs.createWriteStream("./metamod/resourcemod.vdf");
    https.get(
      "https://s3.eu-central-1.amazonaws.com/saido.static/vdfs/resourcemod.vdf",
      function (response) {
        response.pipe(resourcemod);
        resourcemod.on("finish", () => {
          resourcemod.close();
          console.log("[ResourceMod] Download Completed!");
        });
      }
    );

    try {
      if (tag) {
        execSync(`git clone --depth 1 --branch ${tag} ${repository} resourcemod`);
      } else {
        execSync(`git clone --depth 1 ${repository} resourcemod`);
      }
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
        recursive: true,
      });
    } catch (e) {
      console.error("Failed to remove .git dependencies.");
      return false;
    }

    rl.close();
  }
);
