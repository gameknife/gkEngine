/**
 * Created by kaimingyi on 2016/11/15.
 */
const fs        = require('fs-extra');
const fswalker  = require('klaw-sync');
const path      = require('path');

class BaseResObj {
    constructor(token) {
        this.filetoken = token;
        this.loaded = false;

        this.type = "unknown";

        let ext = this.getFileExtension(token);
        if (ext !== undefined) {
            ext = ext.toLowerCase();
            switch (ext) {
                case 'fbx':
                case 'osgjs':
                case 'obj':
                    this.type = "mesh";
                    break;
                case 'jpg':
                case 'png':
                case 'bmp':
                    this.type = "texture";
                    break;
                case 'glsl':
                case 'js':
                    this.type = "script";
                    break;
                case 'mat':
                    this.type = "material";
                    break;
            }
        }
    }

    getFileExtension(filename) {
        return (/[.]/.exec(filename)) ? /[^.]+$/.exec(filename)[0] : undefined;
    }

    get_type() {
        return this.type;
    }
}

export default class EditorResourceManager {
    constructor() {

        this.resFolderStruct = { '_folded' : false, '_name': 'root', '_child' : {} };
        this.resContainer = null;
        this.basedir = null;
        this.curr_selection = null;
        this.resrefs = new Map();

    }

    init(container, basedir) {
        this.resContainer = container;
        this.basedir = basedir;
    }

    add_res(token) {
        token = path.normalize(token);
        if (this.resrefs.has(token)) {
            return;
        }

        let retRes = new BaseResObj(token);
        
        if (retRes !== null && retRes.get_type() != "unknown") {
            this.resrefs.set(token, retRes);
        }
    }

    get_res(token) {
        token = path.normalize(token);
        return this.resrefs.get(token);
    }

    rescan_resources() {
        let filelist = fswalker(this.basedir + '/exec/media/');

        let thisPt = this;

        filelist.forEach( item => {
            let itempath = item.path;
            itempath = path.normalize(itempath);
            let filename = path.basename( itempath );
            let dirname = path.dirname(itempath);
            let relpath = path.relative( thisPt.basedir, dirname );
            let token = path.normalize( relpath + path.sep + filename );
    
            // 添加到资源索引
            thisPt.add_res(token);
        });
    }


    reconstruct_filetree () {

        // rebuild filetree with root
        this.resFolderStruct = { '_folded' : false, '_name': 'root', '_child' : {} };
    
        let thisPt = this;

        // build the folder struct
        this.resrefs.forEach( item => {
    
            let folders = item.filetoken.split(path.sep);
            let currFolderLevel = thisPt.resFolderStruct;

            // go check the folder links, choose the last
            for( let i=0; i < folders.length; ++i)
            {
                if( i === folders.length - 1)
                {
                    // leaf
                    currFolderLevel._child[folders[i]] = item;
                    break;
                }
    
                if( folders[i] in currFolderLevel._child )
                {
                    currFolderLevel = currFolderLevel._child[folders[i]];
                }
                else
                {
                    currFolderLevel._child[folders[i]] = { '_folded' : true, '_child' : {} };
                    currFolderLevel._child[folders[i]]._name = folders[i];
    
                    currFolderLevel = currFolderLevel._child[folders[i]];
                }
            }
        });
    }


    refresh () {
        this.clean_folder(this.resContainer);
        this.list_folder(this.resFolderStruct, 1);
    }



    create_res_showobj(depth, filetoken, type, thumbMode) {

        var obj_container = document.createElement('div');
        // indent
        obj_container.style.padding = '0px ' + depth + 'px';

        // create inside button
        var obj_button = document.createElement('button');
        obj_button.className = 'obj-line';
        obj_button.id = 'btn-' + filetoken;

        // create icon
        var log_icon = null;

        switch (type) {
            case "mesh":
                log_icon = document.createElement('i');
                log_icon.className = 'btm bt-database';
                break;
            case "texture":
                // load resmgt
                let res = this.get_res(filetoken);
                if(res !== null && !res.dynamic)
                {
                    log_icon = document.createElement('img');
                    log_icon.src = res.filetoken;
                    if(thumbMode)
                    {
                        log_icon.width = '13';
                        log_icon.height = '13';
                    }
                    else
                    {
                        log_icon.width = '48';
                        log_icon.height = '48';
                    }

                }
                else
                {
                    log_icon = document.createElement('i');
                    log_icon.className = 'bts bt-photo';
                }
                break;
            default:
                log_icon = document.createElement('i');
                log_icon.className = 'btm bt-file';
                break;
        }
        log_icon.style.color = '#8af';

        // create name text
        var log_text = document.createTextNode(' ' + filetoken.split(path.sep).pop());

        // compose
        obj_button.appendChild(log_icon);
        if(thumbMode)
        {
            obj_button.appendChild(log_text);
        }
        obj_container.appendChild(obj_button);

        // return
        return {obj_container: obj_container, obj_button: obj_button, obj_icon: log_icon};
    }





    list_folder (folderStruct, depth) {

        let thisPt = this;

        for (let element in folderStruct._child) {
    
            let folder = folderStruct._child[element];
            if(folder === undefined)
            {
                break;
            }
    
            if (folder instanceof BaseResObj)
            {
                // create obj button
                let filetoken = folder.filetoken;
                let type = folder.get_type();
                let __ret = this.create_res_showobj(depth, filetoken, type, true);
                let obj_container = __ret.obj_container;
                let obj_button = __ret.obj_button;
                let obj_icon = __ret.obj_icon;
    
                if(folder.loaded)
                {
                    obj_icon.style.color = '#7f7';
                }
    
                // selection status
                if (this.curr_selection === folder) {
                    obj_button.className = 'obj-line-checked';
                }

                // click selection change
                obj_button.onclick = function () {
                    thisPt.curr_selection = folder;
                    thisPt.refresh();
                }
    
                // drag status
                obj_button.draggable = true;
                obj_button.ondragstart = function (ev) {
                    ev.dataTransfer.setData("restoken", filetoken);
                }
    
                // db click
                obj_button.ondblclick = function () {
                    //window.parent.renderer.updateMesh(folder.filetoken);
                }
    
                // append
                this.resContainer.appendChild(obj_container);
            }
            else
            {
                var obj_container = document.createElement('div');
                obj_container.style.padding = '0px ' + depth + 'px';
    
                var obj_button = document.createElement('button');
                obj_button.id = 'folder-' + folder._name;
                obj_button.className = 'obj-line';
    
                obj_button.onclick = function () {
                    folder._folded = !folder._folded;
                    thisPt.refresh();
                }
    
                var log_icon = document.createElement('i');
                if (folder._folded) {
                    log_icon.className = 'bts bt-folder';
                } else {
                    log_icon.className = 'btm bt-folder';
                }
                log_icon.style.color = '#8af';
                var log_text = document.createTextNode(' ' + folder._name);
    
                // compose
                obj_button.appendChild(log_icon);
                obj_button.appendChild(log_text);
                obj_container.appendChild(obj_button);
    
                this.resContainer.appendChild(obj_container);
    
                if (!folder._folded) {
                    this.list_folder(folder, depth + 10);
                }
            }
        }
    }

    clean_folder(resContainer) {
        while (resContainer.firstChild) {
            resContainer.removeChild(resContainer.firstChild);
        }
    }

}






