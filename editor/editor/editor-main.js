let addon = require('../node_natives/build/Release/addon.node');
let path = require('path');

import EditorResourceManager from './respanel';
import Logger from './logger';

window.onload = function(){

  // 初始化canvas
  let canvas = bid('canvas');
  canvas.width = 1280;
  canvas.height = 720;
  let context = canvas.getContext('2d');

  // 设置资源路径
  let rootPath = path.dirname(__dirname);
  let dllPath = rootPath + '.\\..\\exec\\bin64\\';
  let execPath = rootPath + '.\\..\\exec\\';
  addon.setDllPath(rootPath);
  console.log("dll: " + dllPath);
  console.log("exec: " + execPath);
  //return;

  // 初始化渲染器

  addon.initrender(execPath);
  
  Logger.init(bid('console-log-container'));

  //addon.setLogCallback( function(msg) {
  //  Logger.info(msg);
  //} );

  // 创建backbuffer
  let bufferData = Buffer.alloc(1280*720*4);
  let imageData = context.createImageData(1280, 720);

  // 取得profile数据显示器
  let profiledata = bid('profiledata');

  // meshfilter容器逻辑
  let holder = bid('mesh-holder');
  holder.ondrop = function ( ev ) {
    ev.preventDefault();
    let filetoken = ev.dataTransfer.getData("restoken");

    // 拖拽到meshfilter时，向渲染引擎发送设置模型的消息
    let event = "set_model";
    let ret = addon.sendEvent(event, filetoken);
  }
  holder.ondragover = function (ev) {
    ev.preventDefault();
  }

  // let resPanel = new EditorResourceManager();

  // // 刷新project面板
  // resPanel.init(bid('res-container'), path.join(__dirname, '../'));
  // resPanel.rescan_resources();
  // resPanel.reconstruct_filetree();
  // resPanel.refresh();

  // 创建逻辑循环
  render_loop();
  function render_loop()
  {
      // 渲染引擎更新
      addon.rendertobuffer(bufferData);
      // 更新到html
      imageData.data.set(bufferData);
      context.putImageData(imageData, 0, 0);


      // 取得渲染引擎profile数据
      let data = addon.getprofiledata();
      // 设置profile数据
      profiledata.innerHTML=data;

      // 申请下一次循环
      requestAnimationFrame(render_loop);
  }

  // 关闭渲染引擎
  //addon.shutdownrender();
};

function bid(id){return document.getElementById(id);}

