cd ../global_task/
source set_global_env.sh
rm `find $GKENGINE_HOME -type f -name "*.gmf"`
#for /R %GKENGINE_HOME% %%A in (*.obj) do (
#    
#    IF NOT EXIST %%~dpnA.gmf (
#    	echo [convert file] %%~nxA to %%~nA.gmf
#    	%GKENGINE_HOME%\tools\gkResourceCompiler -i %%~fA -o %%~dpnA.gmf -l 5 -p 0-i
#    ) ELSE (
#    	echo [file exist] %%~nA.gmf
#    )
#
#)

# pause