------------------------------------
-- gMaxPort v2.0
-- Basic Exporter Module v1.0
-- by Sascha Herfort
------------------------------------

--###########################################################################################
--MATERIAL EXPORTER STRUCT
--MATERIAL EXPORTER STRUCT   ########################################################################
--MATERIAL EXPORTER STRUCT
--###########################################################################################
struct gMaxPortMaterialExportStruct
(--material exporter/importer struct
--#########################################################
--GLOBAL VARIABLES
--#########################################################
	sMaterialExportFailMessage = ("Material % is not a standard material with a crytek shader. Export failed."), --format with material's name to stringStream
	sMaterialExportSuccessMessage = ("Material file \"%.mtl\" successfully written."), --format with material's name to stringStream
	
--#########################################################
--MATERIAL EXPORT FUNCTIONS
--#########################################################
	fn fnExportMaterial sMaterial bSilent:false =
	(--exports material to .mtl file - non-cryShader materials will cause error - returns true on success, false otherwise
		local sMaterialName = try ("\"" + sMaterial.name + "\"") catch "undefined"
		--gMaxPort.export.log.fnDebugMessage ("MATERIAL EXPORTER: Called to export material: " + sMaterialName + ".")
		
		--local aInvalidMaterials = gMaxPort.export.material.fnGetInvalidMaterialsFromMaterial sMaterial
		
		--if aInvalidMaterials.count == 0 then 
		(--proceed if material is valid
			dotNet.loadAssembly "system.xml" --load dotNet XML assembly 
			
			local xmlMaterialFile = dotNetObject "system.xml.xmlDocument" --create XML document
			
			if classOf sMaterial == multiMaterial then
			(--create multi material
				--create multi material root
				local sXMLRoot = xmlMaterialFile.createElement "Material"
				sXMLRoot.setAttribute "Name" sMaterial.name
				sXMLRoot.setAttribute "MtlFlags" "524544"
				xmlMaterialFile.appendChild sXMLRoot
				
				--create submaterials element
				local sSubMaterials = xmlMaterialFile.createElement "SubMaterials"
				
				for i = 1 to sMaterial.count do
				(--get colors and textures from all submaterials
					local sSubMaterial = gMaxPort.export.material.fnGenerateXMLMaterialBlock sMaterial[i] xmlMaterialFile
					
					sSubMaterials.appendChild sSubMaterial
				)
				
				sXMLRoot.appendChild sSubMaterials
				
				xmlMaterialFile.save (maxFilePath + (sMaterial.name) + ".mtl")
				
				if not bSilent then
				(
					--local ssUserMessage = "" as stringStream
					--format gMaxPort.export.material.sMaterialExportSuccessMessage sMaterial.name to:ssUserMessage
					--gMaxPort.export.log.fnUserNotification (ssUserMessage as string) --user notification
				)
			)
			else
			(--create simple material
				--create single material root
				local sXMLRoot = gMaxPort.export.material.fnGenerateXMLMaterialBlock sMaterial xmlMaterialFile
				
				xmlMaterialFile.appendChild sXMLRoot
				
				xmlMaterialFile.save (maxFilePath + (sMaterial.name) + ".mtl")
				
				if not bSilent then
				(
					--local ssUserMessage = "" as stringStream
					--format gMaxPort.export.material.sMaterialExportSuccessMessage sMaterial.name to:ssUserMessage
					--gMaxPort.export.log.fnUserNotification (ssUserMessage as string) --user notification
				)
			)
			
			true
		)
	),
	
	fn fnGenerateXMLMaterialBlock sMaterial xmlMaterialFile bWriteTexturePaths:true =
	(--generates a block for the MTL containing one material - assumes 3dsmax Standard material, dotNet xmlDocument instance as input
		local sNewMaterial = xmlMaterialFile.createElement "Material"
		sNewMaterial.setAttribute "Name" sMaterial.name
		
		-- 常规属性绑定
		(--set properties for standard illum shader
			sNewMaterial.setAttribute "MtlFlags" "524416"
			
			-- Shader
			sNewMaterial.setAttribute "Shader" "Illum"
			
			-- ShaderMask
			sNewMaterial.setAttribute "GenMask" "100000000"
		)
		
		
		-- 共有属性绑定
		--sNewMaterial.setAttribute "SurfaceType" ""
		--sNewMaterial.setAttribute "MatTemplate" ""
		sNewMaterial.setAttribute "Diffuse" ((sMaterial.g_MatDiffuse.red/255) as string + "," + (sMaterial.g_MatDiffuse.green/255) as string + "," + (sMaterial.g_MatDiffuse.blue/255) as string)
		sNewMaterial.setAttribute "Specular" ((sMaterial.g_MatSpecular.red/255) as string + "," + (sMaterial.g_MatSpecular.green/255) as string + "," + (sMaterial.g_MatSpecular.blue/255) as string)
		sNewMaterial.setAttribute "Emissive" "1.0,1.0,1.0"
		sNewMaterial.setAttribute "Shininess" (sMaterial.g_Gloss as string)
		sNewMaterial.setAttribute "Opacity" "1.0"
		
		
		-- 纹理属性绑定, 暂时关闭
		if false then
		(--write texture paths to MTL file
			local sTextures = xmlMaterialFile.createElement "Textures"
			
			if classOf sMaterial.diffuseMap == bitmapTexture then
			(--if map is a texture
				local sDiffuseTexturePath = undefined
				
				try
				(
					sDiffuseTexturePath = sMaterial.diffuseMap.fileName
				)
				catch
				(
					sDiffuseTexturePath = ""
				)
				
				if matchPattern sDiffuseTexturePath pattern:(gMaxPort.basic.vars.buildPath + "\\game*") then
				(--texture is in game folder structure already - extract path relative to build path
					sDiffuseTexturePath = substring sDiffuseTexturePath (gMaxPort.basic.vars.buildPath + "\\game\\").count (sDiffuseTexturePath.count - gMaxPort.basic.vars.buildPath.count - 5 )
				)
				else if matchPattern sDiffuseTexturePath pattern:(maxFilePath + "*") then
				(--texture is in a subfolder of object - extract relative path
					sDiffuseTexturePath = substring sDiffuseTexturePath (maxFilePath.count + 1) (sDiffuseTexturePath.count - maxFilePath.count)
				)
				else
				(
					sDiffuseTexturePath = undefined
				)
				if sDiffuseTexturePath != undefined then
				(
					local sDiffuseTexture = xmlMaterialFile.createElement "Texture"
					sDiffuseTexture.setAttribute "Map" "Diffuse"
					sDiffuseTexture.setAttribute "File" sDiffuseTexturePath
					sTextures.appendChild sDiffuseTexture
				)
			)
			
			if classOf sMaterial.specularMap == bitmapTexture then
			(--if map is a texture
				local sSpecularTexturePath = sMaterial.specularMap.fileName
				if matchPattern sSpecularTexturePath pattern:(gMaxPort.basic.vars.buildPath + "\\game*") then
				(--texture is in game folder structure already - extract path relative to build path
					sSpecularTexturePath = substring sSpecularTexturePath (gMaxPort.basic.vars.buildPath + "\\game\\").count (sSpecularTexturePath.count - gMaxPort.basic.vars.buildPath.count - 5 )
				)
				else if matchPattern sSpecularTexturePath pattern:(maxFilePath + "*") then
				(--texture is in a subfolder of object - extract relative path
					sSpecularTexturePath = substring sSpecularTexturePath (maxFilePath.count + 1) (sSpecularTexturePath.count - maxFilePath.count)
				)
				else
				(
					sSpecularTexturePath = undefined
				)
				if sSpecularTexturePath != undefined then
				(
					local sSpecularTexture = xmlMaterialFile.createElement "Texture"
					sSpecularTexture.setAttribute "Map" "Specular"
					sSpecularTexture.setAttribute "File" sSpecularTexturePath
					sTextures.appendChild sSpecularTexture
					--sSpecularTexture.appendChild (xmlMaterialFile.createElement "TexMod")
				)
			)
			
			if classOf sMaterial.bumpMap == normal_bump and classOf sMaterial.bumpMap.normal_map == bitmapTexture then
			(--if normalbump is used
				local sBumpTexturePath = sMaterial.bumpMap.normal_map.fileName
			)
			else if classOf sMaterial.bumpMap == bitmapTexture then
			(
				local sBumpTexturePath = sMaterial.bumpMap.fileName
			)
			if sBumpTexturePath != undefined then
			(
				if matchPattern sBumpTexturePath pattern:(gMaxPort.basic.vars.buildPath + "\\game*") then
				(--texture is in game folder structure already - extract path relative to build path
					sBumpTexturePath = substring sBumpTexturePath (gMaxPort.basic.vars.buildPath + "\\game\\").count (sBumpTexturePath.count - gMaxPort.basic.vars.buildPath.count - 5 )
				)
				else if matchPattern sBumpTexturePath pattern:(maxFilePath + "*") then
				(--texture is in a subfolder of object - extract relative path
					sBumpTexturePath = substring sBumpTexturePath (maxFilePath.count + 1) (sBumpTexturePath.count - maxFilePath.count)
				)
				else
				(
					sBumpTexturePath = undefined
				)
				if sBumpTexturePath != undefined then
				(
					local sBumpTexture = xmlMaterialFile.createElement "Texture"
					sBumpTexture.setAttribute "Map" "Bumpmap"
					sBumpTexture.setAttribute "File" sBumpTexturePath
					sTextures.appendChild sBumpTexture
				)
			)
			
			sNewMaterial.appendChild sTextures
		)
		
		-- shader属性绑定 shaderparams
		local sPublicParams = xmlMaterialFile.createElement "PublicParams"
		sPublicParams.setAttribute "AmbientMultiplier" "1"
		sNewMaterial.appendChild sPublicParams
		
		sNewMaterial --return result
	),
	
	
--#########################################################
--MATERIAL IMPORT FUNCTIONS
--#########################################################
	fn fnImportMaterial sFile: =
	(--import .mtl file to material
		dotNet.loadAssembly "system.xml"
		
		if sFile == unsupplied then
		(
			sFile = getOpenFileName caption:"Please specify material file!" fileName:maxFilePath types:"Crysis Material File (*.mtl)|*.mtl"
		)
		
		if sFile != undefined then
		(--load material file
			local xmlMaterialFile = dotNetObject "system.xml.xmlDocument"
			xmlMaterialFile.load sFile
			
			--load root
			local sXMLRoot = xmlMaterialFile.documentElement
			if sXMLRoot != undefined then
			(
				max mtledit
				local sMaterial = medit.GetCurMtl()
				sMaterial.name = getFilenameFile sFile
				
				if sXMLRoot.ChildNodes.itemOf[0] != undefined and sXMLRoot.ChildNodes.itemOf[0].name == "SubMaterials" then
				(
					sMaterial = multimaterial numSubs:sXMLRoot.ChildNodes.itemOf[0].ChildNodes.count
					sMaterial.name = getFilenameFile sFile
					setMeditMaterial (medit.GetActiveMtlSlot()) sMaterial
					
					for i = 0 to sXMLRoot.ChildNodes.itemOf[0].ChildNodes.count - 1 do
					(
						local sSubMaterial = sXMLRoot.ChildNodes.itemOf[0].ChildNodes.itemOf[i]
						
						sMaterial.materialList[i+1].name = (sSubMaterial.getAttributeNode "Name").value
						sMaterial.materialList[i+1].shaderType = 2
						
						local aTempColor = filterString (sSubMaterial.getAttributeNode "Diffuse").value ","
						sMaterial.materialList[i+1].diffuse = color (aTempColor[1] as float *255) (aTempColor[2] as float *255) (aTempColor[3] as float *255)
						
						local aTempColor = filterString (sSubMaterial.getAttributeNode "Specular").value ","
						sMaterial.materialList[i+1].specular = color (aTempColor[1] as float *255) (aTempColor[2] as float *255) (aTempColor[3] as float *255)
						sMaterial.materialList[i+1].specularLevel= 100
						
						sMaterial.materialList[i+1].opacity = ((sSubMaterial.getAttributeNode "Opacity").value as float)*100
						sMaterial.materialList[i+1].alphaBlend = (sMaterial.materialList[i+1].opacity != 100)
						
						for j = 0 to sSubMaterial.childNodes.itemOf[0].childNodes.count-1 do
						(--load texture filenames
							local sTexture = sSubMaterial.childNodes.itemOf[0].childNodes.itemOf[j]
							print (sTexture.getAttributeNode "Map").value
							case (sTexture.getAttributeNode "Map").value of
							(
								"Diffuse":
								(
									sMaterial.materialList[i+1].diffuseMapEnable = on
									sMaterial.materialList[i+1].diffuseMap = Bitmaptexture fileName:(gMaxPort.basic.vars.buildPath + "\\Game\\" + (sTexture.getAttributeNode "File").value)
								)
								"Bumpmap":
								(
									sMaterial.materialList[i+1].normalMapEnable = on
									sMaterial.materialList[i+1].normalMap = Bitmaptexture fileName:(gMaxPort.basic.vars.buildPath + "\\Game\\" + (sTexture.getAttributeNode "File").value)
								)
								"Specular":
								(
									sMaterial.materialList[i+1].specularMapEnable = on
									sMaterial.materialList[i+1].specularMap = Bitmaptexture fileName:(gMaxPort.basic.vars.buildPath + "\\Game\\" + (sTexture.getAttributeNode "File").value)
								)
								"SubSurface":
								(
									sMaterial.materialList[i+1].subsurfaceMapEnable = on
									sMaterial.materialList[i+1].subsurfaceMap = Bitmaptexture fileName:(gMaxPort.basic.vars.buildPath + "\\Game\\" + (sTexture.getAttributeNode "File").value)
								)
							)
						)
					)
				)
			)
		)
	)
)

--###########################################################################################
--GENERAL EXPORTER STRUCT
--GENERAL EXPORTER STRUCT #########################################################################
--GENERAL EXPORTER STRUCT
--###########################################################################################
struct gMaxPortExportStruct
(
--#########################################################
--GLOBAL VARIABLES
--#########################################################
	--object = gMaxPortObjectExportStruct(),
	--anim = gMaxPortAnimationExportStruct(),
	material = gMaxPortMaterialExportStruct()
	--log = gMaxPortExporterLogMessageStruct(),
	
)

gMaxPort.export = gMaxPortExportStruct()

--callbacks to update and global exporter data when file opened/merged/imported/created or max reseted
--callbacks.addScript #filePostOpen "gMaxPort.export.fnUpdate()" id:#updateExporterData
--callbacks.addScript #filePostMerge "gMaxPort.export.fnUpdate()" id:#updateExporterData
--callbacks.addScript #postImport "gMaxPort.export.fnUpdate()" id:#updateExporterData
--callbacks.addScript #systemPostNew "gMaxPort.export.fnUpdate()" id:#updateExporterData
--callbacks.addScript #systemPostReset "gMaxPort.export.fnUpdate()" id:#updateExporterData


--csexport.export.register_export_callback gMaxPort.export.fnCheckSceneForOldExport
