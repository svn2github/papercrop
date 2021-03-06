require('mylib')
require('config')


function processOption(option)
	option=string.gsub(option,"%) ", ")\n");
	option=string.gsub(option," %(", " \n(");
	return option;
end

function unprocessOption(option)
	option=string.gsub(option, "%)\n", ") ");
	option=string.gsub(option, " \n%(", " (");
	return option
end
function loadPreset(fn)
	local fno,msg=loadfile(fn)
	if not fno then print(msg) return end
	fno()
	local layout=panel:findLayout("Automatic segmentation");
	layout:findWidget("MIN gap"):sliderValue(MIN_gap);
	layout:findWidget("Margin"):sliderValue(margin);
	layout:findWidget("N columns"):sliderValue(N_columns);
	layout:findWidget("white point"):sliderValue(white_point);
	layout:findWidget("Crop T"):sliderValue(crop_T);
	layout:findWidget("Crop B"):sliderValue(crop_B);
	layout:findWidget("Crop L"):sliderValue(crop_L);
	layout:findWidget("Crop R"):sliderValue(crop_R);
	panel:findWidget("Option_Input"):inputValue(processOption(option));

	local options=panel:findWidget("options")
	local argMenu=table.find(goptions, option)-1
	--print("argMenu", argMenu)
	options:menuValue(argMenu)
	panel:redraw();
end
function processPresetName(fn)
	return string.sub(os.processFileName(fn), 1,-5)
end
function processOptionName(fn)
	return string.sub(os.processFileName(fn), 1,-5)
end
function setPresetMenu()
	local presetMenu=panel:findWidget('presets')
	-- load preset
	presets=os.glob("presets/*.lua")
	local function comp(a,b)
		local fn=processPresetName(a)
		if fn=="default preset" then return true end
		return a<b
	end

	table.sort(presets, comp)
	if device.supported_presets then
		device.supported_presets['default preset']=true
		presets=array.filter(function(f) return device.supported_presets[processPresetName(f)] end,
		presets)
	end
	presetMenu:menuSize(#presets)
	for i=1,#presets do
		local shortc
		if i-1<10 then
			shortc="FL_ALT+"..tostring(i-1)
		end
		local preset=processPresetName(presets[i])
		presetMenu:menuItem(i-1,preset ,shortc)
	end
	presetMenu:menuValue(0)
end
function setOptionMenu()
	local optionMenu=panel:findWidget('options')
	-- load option
	options=os.glob("script/*.lua")
	--printTable(options)
	local function comp(a,b)
		return a<b
	end

	table.sort(options, comp)
	if device.supported_options then
		options=array.filter(function(f) return device.supported_options[processOptionName(f)] end,
		options)
	end
	for i=1, #options do
		options[i]=processOptionName(options[i])
	end
	optionMenu:menuSize(#options)
	for i=1,#options do
		optionMenu:menuItem(i-1,options[i])
	end
	optionMenu:menuValue(0)
	goptions=options
end

function ctor()
	panel:create("Button", "Load a PDF file", "Load a PDF file");

	panel:widget(0):buttonShortcut("FL_ALT+l");
	panel:widget(0):buttonTooltip("Alt+L");

	if os.isUnix() then
		panel:create("Button", "Load a PDF file (native)", "Load a PDF file (native)");

		panel:widget(0):buttonShortcut("FL_ALT+n");
		panel:widget(0):buttonTooltip("Alt+N");
	end
	panel:create("Choice", "presets")
	--panel:create("Button", "Preset", "default preset");
	setPresetMenu();
		
	-- load devices
	panel:create("Choice", "devices")
	local ndevices=0
	local idevice=0
	gdevices={}
	for k,v in pairs(devices) do 
		ndevices=ndevices+1 
		gdevices[ndevices]={k,v}
		if device==v then 
			idevice=ndevices 
		end 
	end
	panel:widget(0):menuSize(ndevices+1)
	panel:widget(0):menuItem(0, 'choose a device')
	for i,v in ipairs(gdevices) do
		panel:widget(0):menuItem(i, v[1])
	end
	panel:widget(0):menuValue(idevice)

	panel:create("Check_Button", "Use automatic segmentation", "Use automatic segmentation");
	panel:widget(0):checkButtonValue(1);


	panel:create("Layout", "Automatic segmentation", "segmentation parameters");
	panel:layout(0):create("Value_Slider", "MIN gap","MIN gap", 1);
	panel:layout(0):widget(0):sliderRange(0.0, 10.0);
	panel:layout(0):create("Value_Slider", "MIN text-gap","MIN text-gap");
	panel:layout(0):widget(0):sliderRange(1.0, 10.0);
	panel:layout(0):widget(0):sliderValue(2.0);
	panel:layout(0):create("Value_Slider", "Margin","Margin");
	panel:layout(0):widget(0):sliderRange(0.5, 10.0);
	panel:layout(0):create("Value_Slider", "N columns","N columns");
	panel:layout(0):widget(0):sliderRange(0.7, 10);
	panel:layout(0):widget(0):sliderStep(0.1);
	panel:layout(0):create("Value_Slider", "white point","white point");
	panel:layout(0):widget(0):sliderRange(230, 255);
	panel:layout(0):widget(0):sliderStep(1);

	panel:layout(0):create("Value_Slider", "Crop T","Crop Top");
	panel:layout(0):widget(0):sliderRange(0, 20);
	panel:layout(0):widget(0):sliderStep(0.1);
	panel:layout(0):create("Value_Slider", "Crop B","Crop Bttm");
	panel:layout(0):widget(0):sliderRange(0, 20);
	panel:layout(0):widget(0):sliderStep(0.1);
	panel:layout(0):create("Value_Slider", "Crop L","Crop Left");
	panel:layout(0):widget(0):sliderRange(0, 20);
	panel:layout(0):widget(0):sliderStep(0.1);
	panel:layout(0):create("Value_Slider", "Crop R","Crop right");
	panel:layout(0):widget(0):sliderRange(0, 20);
	panel:layout(0):widget(0):sliderStep(0.1);

	panel:layout(0):create("Button", "update","update");
	panel:layout(0):widget(0):buttonShortcut("FL_ALT+u")
	panel:layout(0):widget(0):buttonTooltip("ALT+u")
	panel:layout(0):updateLayout();
	panel:setWidgetHeight(60);
	panel:setUniformGuidelines(5);
	panel:create("Input", "Option_Input", "Option",1);
	panel:widget(0):inputType("FL_MULTILINE_OUTPUT");
	panel:resetToDefault()
--	panel:create("Button", "Option", "Change option", 1);
	panel:create("Choice", "options","",0)
	setOptionMenu()
	
	panel:create("Button", "Process current page", "Process current page",0);
	panel:widget(0):deactivate();

	if output_to_pdf then
		if device and device.output_format==".cbz" then
			panel:create("Button", "Convert processed pages to PDF", "Convert processed pages to CBZ")
		else
			panel:create("Button", "Convert processed pages to PDF", "Convert processed pages to PDF")
		end
		panel:widget(0):deactivate();
	end

	panel:create("Button", "Process all pages", "Process all pages",0);

	panel:widget(0):deactivate();
	panel:widget(0):buttonShortcut("FL_ALT+p");

	panel:create("Button", "Batch process", "Batch process",0);

	panel:create("Box", "Status");
	panel:updateLayout();

	loadPreset("presets/default preset.lua")

	if filename then
		print(filename)
		win:load(filename)
		filename=nil
	end
	if auto then
		print("Automatic processing:")
		g_automatic_mode=true
		onCallback(panel:findWidget("Process all pages"), nil)
		panel('exit',0)
	end
end
function dtor()
	print('')
end
function Fltk.ChooseFile(title, path, mask, write)
	if write==nil then write=false end
	local fn=Fltk.chooseFile(title, path, mask,write)
	if fn=="" then return nil end
	return fn
end
function setDefaultUI()
	setPresetMenu()
	setDefault()
	setOptionMenu()
end
function onCallback(w, userData)
	if w:id()=="Load a PDF file (native)" then

		local fn=os.capture("python filechooser.py")
		if fn~='Closed, no files selected' then
			print(fn..':')
			win:load(fn)
		end
	elseif w:id()=="devices" then
		local dev=gdevices[w:menuValue()]
		device=dev[2] or device
		print(dev[1]..' selected', dev[2])
		setDefaultUI()	
		loadPreset("presets/default preset.lua")
		win:redraw()
		panel:redraw()
	elseif w:id()=="Convert processed pages to PDF" then
		local outdir=string.sub(win.filename, 1, -5)

		local files=os.glob(outdir.."/*"..output_format)
		printTable(files)
		table.sort(files)

		local outpdf, fn
		local function open(fn)
			if os.isUnix() then
				print('gnome-open "'..fn..'"')
				os.execute('gnome-open "'..fn..'"')
			else
				os.execute('start "'..fn..'"')
			end
		end
		if device and device.output_format==".cbz" then
			fn=outdir.."_1.cbz"
			outpdf=CBZwriter:new(fn)
		elseif device and device.output_format==".xml" then
			fn=outdir.."_1.xml"
			outpdf=XMLwriter:new(fn,book_pages, outdir)
			outpdf:init()
			outpdf:addPage()
			outpdf:save(fn)
			open(string.sub(fn,1,-4).."pdf")
			return
		else
			outpdf=PDFWriter()
			fn=outdir.."_1.pdf"
		end
		outpdf:init()

		if not os.isUnix() then
			for i=1,#files do
				files[i]=string.gsub(files[i],"/","\\")
			end
		end


		for i=1,#files do
			print(files[i])
			local image=CImage()
			image:Load(files[i])
			if color_depth>8 then
				outpdf:addPageColor(image)
			else
				outpdf:addPage(image, color_depth)
			end
			collectgarbage();
		end
		outpdf:save(fn)
		open(fn)
		collectgarbage();
	elseif w:id()=="presets" then
		local fn=presets[w:menuValue()+1]
		local filename=os.processFileName(fn)
		--w:buttonSetLabel(string.sub(filename,1, -5))
		loadPreset(fn)
		win:pageChanged();
		win:redraw();
		panel:redraw();
		return true
	elseif (w:id()=="Option") then
		local fn=Fltk.ChooseFile("Choose option", "script", "*.lua");
		if fn then
			local fn2=os.filename(fn)
			if device.supported_options then
				if not device.supported_options[fn2] then
					util.msgBox('Option "'..string.sub(fn2,1,-5)..'" is not supported for '..(device.device_name or "the device").."!")	
					return true
				end
			end
			panel:findWidget("Option_Input"):inputValue(processOption(string.sub(fn, 1,-5)))
		end
		--print('hihi')
		return true
	elseif (w:id()=="options") then
		local menuText=w:menuText()
		panel:findWidget("Option_Input"):inputValue(processOption(menuText))
		--print('hihi')
		return true
	elseif(w:id()=="Process current page" or  w:id()=="Process all pages") then
		panel:findWidget("Process current page"):deactivate();
		panel:findWidget("Process all pages"):deactivate();
	
		local bAll=w:id()=="Process all pages";
		local fn="script/"..unprocessOption(panel:findWidget("Option_Input"):inputValue())..".lua";

		local fno,msg=loadfile(fn)
		if not fno then
			print(fn)
			print(msg)
			return false
		end
		fno()

		local outdir=string.sub(win.filename, 1, -5)

		if bAll then
			if not output_to_pdf then
				CreateDirectory(outdir);
			end
			processAllPages(outdir)
		else
			CreateDirectory(outdir);
			processPage(outdir, win.currPage, win:getNumRects())
			if output_to_pdf then
				panel:findWidget("Convert processed pages to PDF"):activate();
			end
		end

		win:setStatus("processing ended")
		panel:findWidget("Process current page"):activate();
		panel:findWidget("Process all pages"):activate();
		return true
	end
	return false
end
