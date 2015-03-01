--//////////////////////////////////////////////////////////////////////////
--//
--// K&K Studio GameKnife ENGINE Source File
--//
--// Name:   	GMPToolbox.ms
--// Desc:		
--// 
--// Author:  Kaiming
--// Date:	2011/9/28 
--// Modify:	2011/9/28
--// 
--//////////////////////////////////////////////////////////////////////////
struct MaxPortStruct
(
		--// tools \\--
	export, --// exporter tools
	
		--// basic functionality \\--
	--basic = cryMaxToolsBasicStruct(),
	toolBox,
	
		--// shared functions \\--
	getchildren, --returns all nodes in hierarchy below this node
	findRoot, --returns root of hierarchy above this node
	getBips, --returns all bip roots in a scene
	
	rootPath,
	
	fn getAbsPath relPath =
	(
		local path = (getdir #maxroot) + "Scripts/gkMaxPort/" + relPath
		return path
	)
)

global gMaxPort = MaxPortStruct()
gMaxPort.rootPath =  #maxroot "Scripts/gkMaxPort/"

mat_id = 0

-- subrollout for scene access
rollout rltSubRolloutShader "材质Shader"
(
	local bm = openBitMap (gMaxPort.getAbsPath "rbShader.png")
	imgtag img_tag "imgtag" bitmap:bm align:#center
	
	-- todo: read fx from given path into items
	dropDownList ddlShader "当前Shader" \
	items:#("E:\TitanMaxPort\ShaderLibrary\Illum.fx",
		"E:\TitanMaxPort\ShaderLibrary\Character10618.fx"
		) height:6
	
	button btnOpenMtl "打开材质" height:20 width:150 
	button btnExportMtl "导出材质" height:20 width:150 
	button btnImportMtl "导入材质" height:20 width:150 
	
	
	on btnExportMtl pressed do
	(
		if ( selection[1] == undefined ) then
		(
			messagebox "请选择你要打开的物体！"
			false
		)
		else if ( selection[1].material == undefined ) then
		(
			messagebox "材质不服规范！"
			false			
		)
		else if ( ClassOf(selection[1].material) != DxMaterial ) then
		(
			messagebox "材质不服规范！"
			false			
		)
		else
		(
			gMaxPort.export.material.fnExportMaterial selection[1].material
		)
		
		
	)


	on btnImportMtl pressed do
	(

	)
	
	on btnOpenMtl pressed do
	(
		if ( selection[1] == undefined ) then
		(
			messagebox "请选择你要打开的物体！"
			--return
		)
		else
		(
			MatEditor.Open()
			medit.PutMtlToMtlEditor selection[1].material 1 --add it to slot 3
			medit.SetActiveMtlSlot 1 true --set slot 3 as the active slot
		)
		
	)
	
	
	on ddlShader selected sel do
	(
		for obj in selection do
		(
			if( ClassOf( obj.material ) == DxMaterial ) then
			(
				print ddlShader.items[sel]
				obj.material.effectfile = ddlShader.items[sel]
				
				-- todo: basic defaut value assgin to new shader
			)
		)
	)
		
)

	function TranslateMtl mtl = 
	(	
			-- 针对一般材质的转换
			if ( classOf(mtl) == Standard) then
			(
				
				local prevname = mtl.name
				
				-- 得到diffusemap和常规参数
				map = mtl.diffuseMap
					diffusecol = mtl.diffuse
					specpow = mtl.Glossiness
					speccol = mtl.specular
				
				-- 处理一般位图
				--if( classOf(map) == BitmapTexture or classOf(map) == Bricks ) then
				(			
					if( classOf(map) == BitmapTexture ) then
					(
						rm = map.bitmap
					)
					else			
					(
						-- 对于位图，进行渲染处理
						print outpath
						--rm = renderMap map size:[512,512] \
						--fileName:outpath
						--save rm
						--close rm
					normalpath = (getdir #maxroot)
					normalpath = pathConfig.appendpath normalpath "Scripts/gkMaxPort/BasicRes/grey.tga"
						
					rm = openBitMap normalpath
					)
				
					-- 处理法线纹理
					normalpath = (getdir #maxroot)
					normalpath = pathConfig.appendpath normalpath "Scripts/gkMaxPort/BasicRes/flat.tga"
					
						
					-- 处理材质shader文件
					outpath = (getdir #maxroot)
					outpath = pathConfig.appendpath outpath "Scripts/gkMaxPort/ShaderLibrary/ksBase.fx"
						
					-- 切换shader
					mtl = DirectX_9_Shader effectfile:outpath
					--mtl.name = myname
					
					
					-- 设置一般属性
					mtl.g_Gloss = 25--specpow
					mtl.g_MatSpecular = color 10 10 10--speccol
					mtl.g_MatDiffuse = diffusecol--color 128 128 128--speccol
					
					-- 设置diffuse
					if( rm != undefined ) then
						mtl.seteffectbitmap 1 rm
						
					-- 设置normalmap
					normalmap = openBitMap normalpath
					mtl.seteffectbitmap 2 normalmap
				
					print "dx9 material created."
				)				
				mtl.name = prevname
			)		
		return mtl
	)
	
	
	fn conutTheMtlID obj =
	(
		faceCount = obj.getnumfaces() --get all the faces number
		tempFaces = 0
		mtlID = #()
		for i in 1 to faceCount where tempFaces < faceCount do
		(
		--start to count
		id = polyOp.getFaceMatID obj i
			if findItem mtlID id == 0 then
			(  append mtlID id
				obj.EditablePoly.selectByMaterial id
				selectFaces = polyOp.getFaceSelection obj
				tempFaces += selectFaces.numberSet
			)
		)
		mtlID
	)
 
	function deatchObjById obj id =
	(
		max modify mode
		subobjectLevel = 4
		obj.EditablePoly.selectByMaterial id
		selectFaces = polyOp.getFaceSelection obj
		
		newName = uniquename ("gkMod" + "_")
		
		polyop.detachFaces obj selectFaces delete:true asNode:true name:newName
		NewObj = getnodebyname newName
		NewObj
	)
	
	function mergeSelection =
	(
		for obj in selection do
		(
			--converttomesh obj
			convertToPoly obj
		)
		
		local objarr = for i in $ collect i
		motherobj = objarr[1]

		local mat_id_count = selection.count
		
		for i = 2 to mat_id_count do
		(	
			polyop.attach motherobj objarr[i]			
		)
		
		--reorder
		
		local newarr = #()
		
		local mtlID = conutTheMtlID $
		for i in 1 to mtlID.count do
		(
			append newarr (deatchObjById $ mtlID[i])
		)
		
		for i in 1 to newarr.count do
		(
			polyop.attach motherobj newarr[i]
		)
	)

	
rollout rltSubRolloutEnvState "资源导入"
(
	--edittext edtSceneName "Current Shader:" fieldWidth:160 labelOnTop:true readOnly:true	
	local bm = openBitMap (gMaxPort.getAbsPath "rbEnvState.png")
	local envbm = Bitmap 128 128 color:black
	imgtag img_tag "imgtag" bitmap:bm align:#center
	
	group "导出信息"
	(
		label geo_file "thinking..." width:150 
		label tex_file "thinking..." width:150 
		label mtl_file "thinking..." width:150 
	)
	
	group "几何导入:"
	(
		button geo_merge "合并所选" width:150	
		button geo_reorder "重组所选" width:150	
		button geo_export "导出obj" width:150		
		button geo_export_gmf "导出gmf" width:150
		button geo_export_obj "导出gk_obj" width:150
	)
	
	group "纹理导入:"
	(	
		button tex_convert "转换纹理" width:150		
	)	
	
	group "材质导入:"
	(
		button mtl_export "导出材质" width:150
	)
	
	
	function init =
	(
		local outpath = maxfilename
		
		outpath = maxfilename
		outpath = replace outpath (outpath.count - 3) 4 ".obj"		
		geo_file.text = outpath
		
		outpath = "texture/" + maxfilename
		outpath = replace outpath (outpath.count - 3) 4 ".tga"		
		tex_file.text = outpath
		
		outpath = maxfilename
		outpath = replace outpath (outpath.count - 3) 4 ".mtl"		
		mtl_file.text = outpath
	)
	
	on geo_merge pressed do
	(
		mergeSelection()
	)
	
	on geo_reorder pressed do
	(
		--reorder
		local mtlID = conutTheMtlID $
		print mtlID
		for i in 1 to mtlID.count do
		(
			deatchObjById $ mtlID[i]
		)
	)
	
	on geo_export pressed do
	(
		theClasses =exporterPlugin.classes
		print theClasses
		print theClasses[16].classid
		--print theClasses[16].classes
		
		print "obj exported."
		--edit (objexp.getIniName())
		theINI =objimp.getIniName()
		setIniSetting theINI "General" "UseLogging" "0"
		setIniSetting theINI "Geometry" "FlipZyAxis" "0"	
		setIniSetting theINI "Geometry" "FaceType" "0"	
		setIniSetting theINI "Geometry" "TextureCoords" "1"	
		setIniSetting theINI "Geometry" "Normals" "1"							
		setIniSetting theINI "Material" "UseMaterial" "1"
		
		outpath = maxfilepath + maxfilename
		outpath = replace outpath (outpath.count - 3) 4 ".obj"		
		print outpath		
		exportFile outpath #noPrompt selectedonly:true  using:13713439701730353346
		
		 
	)
	
	on geo_export_gmf pressed do
	(
		GMPSaveGMFAs()
	)
	
		on geo_export_obj pressed do
	(
		GMPSaveObjAs()
	)
	
	function tex_conv_mtl submtl = 
	(
		local map = submtl.diffuseMap
			
				if( classOf(map) == BitmapTexture) then
				(
					-- use maxroot path
					outpath = (getdir #maxroot)	
					outpath = pathConfig.removePathLeaf outpath
					outpath = pathConfig.appendpath outpath "media/Textures/Temp/"
					outpath = outpath + submtl.name +".tga"
				
					-- use maxfile path
					outpath = maxfilepath
					outpath = pathConfig.appendpath outpath "texture/"
					
					-- creat path
					makeDir outpath all:true
					outpath = outpath + submtl.name +".tga"			
					
					print map.bitmap.filename
					b1 = bitmap map.bitmap.width map.bitmap.height
					print map.bitmap.width as string + " | " + map.bitmap.height as string
					pastebitmap map.bitmap b1 [0,0] [0,0]
					
					--display b1
					
					b1.filename = outpath
					save b1
					
					map.filename = outpath
					map.reload
				)
	)
	
	on tex_convert pressed do
	(
		local mtl = $.material
		if( classOf(mtl) == MultiMaterial )then
		(	
			local subcount = 0;
			for submtl in mtl.materialList do
			(
				submtl.name = mtl.name + "_" + subcount as String
				subcount = subcount + 1
				tex_conv_mtl submtl
			)
		)
		else
		(
			tex_conv_mtl mtl
		)
	)
	
	on mtl_export pressed do
	(
		if( $ != undefined) then
		(
			if( $.material != undefined) then
			(
				local oldmtl = $.material
				local newmtl = copy $.material
				
				if( classof(newmtl) == MultiMaterial ) then
				(					
					for i = 1 to newmtl.materialList.count do
					(
						newmtl.materialList[i] = TranslateMtl newmtl.materialList[i]
					)
					
				)
				else
				(
					newmtl = TranslateMtl newmtl
				)
							
				$.material = newmtl
				
				print "mtl converted"
				
				outpath = maxfilepath + maxfilename
				outpath = replace outpath (outpath.count - 3) 4 ".mtl"		
				print outpath	
				
				
				GMPExportMtl outpath
				
				$.material = oldmtl
			)
			
		)
		 
	)
	
)

materialcount = 1

-- main toolbox holder
rollout rltGkMaxPortToolBox "Titan MaxPort"
(
	
	
	button btnDockLeft "<---" height:16 width:50 pos:[0,0]
	button btnUndock "GMP ToolBox" height:16 width:90 pos:[50,0] 
	button btnDockRight "--->" height:16 width:50 pos:[140,0] 
	
	--bitmap bmpTitle "Bitmap" pos:[0,16] width:190 height:80 fileName:"title.jpg" border:0
	local bm = openBitMap (gMaxPort.getAbsPath "rbMaxPort.png")
	imgtag img_tag "imgtag" bitmap:bm align:#center
	
	subrollout rltToolHolder height:480 width:190 pos:[0,96]
	
	on btnDockLeft pressed do
	(
		try
			(
				local sDockState = cui.getDockState rltGkMaxPortToolBox
			)
			catch
			(--not a registered dialog bar
				local sDockState = undefined
			)
			
			if sDockState == undefined then
			(--register if not yet done
				cui.RegisterDialogBar rltGkMaxPortToolBox style:#() maxsize:[190,1200]
			)
			cui.DockDialogBar rltGkMaxPortToolBox #cui_dock_left
			
			--rltGkMaxPortToolBox.rltToolHolder.height = rltGkMaxPortToolBox .height - rltGkMaxPortToolBox.rltToolHolder.pos[2]
		
	)
	
		
	on btnDockRight pressed do
	(
		try
		(
			local sDockState = cui.getDockState rltGkMaxPortToolBox
		)
		catch
		(--not a registered dialog bar
			local sDockState = undefined
		)
		
		if sDockState == undefined then
		(--register if not yet done
			cui.RegisterDialogBar rltGkMaxPortToolBox style:#() maxsize:[190,1200]
		)
		cui.DockDialogBar rltGkMaxPortToolBox #cui_dock_right
		
		--rltCryMaxToolBox.rltToolHolder.height = rltCryMaxToolBox.height - rltCryMaxToolBox.rltToolHolder.pos[2]
	)
	
	on btnUndock pressed do
	(
		try
		(
			local sDockState = cui.getDockState rltGkMaxPortToolBox 
		)
		catch
		(--not a registered dialog bar
			local sDockState = undefined
		)
		
		if sDockState != undefined then
		(--float and undock if registerd
			cui.FloatDialogBar rltGkMaxPortToolBox 
			cui.UnRegisterDialogBar rltGkMaxPortToolBox 
		)
		
		--rltCryMaxToolBox.rltToolHolder.height = rltCryMaxToolBox.height - rltCryMaxToolBox.rltToolHolder.pos[2]
	)

)

-- initial work
createDialog (rltGkMaxPortToolBox) height:800 width:190 style:#(#style_titlebar, #style_resizing, #style_sysmenu) lockWidth:true


addSubrollout rltGkMaxPortToolBox.rltToolHolder rltSubRolloutShader
addSubrollout rltGkMaxPortToolBox.rltToolHolder rltSubRolloutEnvState 
rltSubRolloutEnvState.init()

--addSubrollout rltGkMaxPortToolBox.rltToolHolder rltSubRolloutTodTask 


fileIn "exporter.ms" quiet:true
