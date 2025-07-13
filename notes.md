# Debugging DLLs or MEX files from MATLAB, MSVC

- if MATLAB is already open, call `clear mex`
- compile code with the /Zi /Od /DDEBUG
- link code with /DEBUG
- open matlab
- get matlab process id
  - in MATLAB 2025a, call `matlabProcessID`
  - in older versions, call `feature getpid`
  - or use taskmgr, but there may be several processes for MATLAB
- from a VS command prompt
  - launch `devenv <name-of-source-file>`
  - Select attach to process, and select the correct matlab pid
  - Set a break point
- from MATLAB, run some code that calls the mex file

- You could use the /debugexe feature of devenv to further automate
  the workload.

# Profiling DLLs or MEX files from MATLAB, MSVC

- You can profile the debug code, but this may not really give you what you want. I recommend profiling the release code.
- To do this, compile with /Zi and whatever optimization setting you want, probably /O2
- Link code with /DEBUG /OPT:NOREF /OPT:NOICF /INCREMENTAL:NO
- You'll get a .pdb file just like with a debug build. It has the info to let the profilier identify the function names. It also produces better stack traces. I just go with this by default, no real penalty.
- Launch MATLAB
- Get matlab process id
  - in MATLAB 2025a, call `matlabProcessID`
  - in older versions, call `feature getpid`
  - or use taskmgr, but there may be several processes
- In a VS command prompt
  - set a variable for the folder containing 'VSDiagnostics.exe'
  - probably `set diagdir=%VSINSTALLDIR%\Team Tools\DiagnosticsHub\Collector`
  - you can confirm the location with `where VSDiagnostics.exe`
  - Call `VSDiagnostics.exe start <SessionID> /attach:<pid> /loadConfig:<configFile>`
    - you choose the session ID in the range [1, 255]. Just stick with 1 unless you're profiling multiple things simultaneously.
    - The `<pid>` is the matlab process pid
    - The `<configFile>` is from the "%diagdir%\AgentConfigs" folder
    - Probably `"%diagdir%\AgentConfigs\CPUUsageBase.json"`
    - Quotes are likely mandatory, as `%diagdir%` likely contains spaces
    - This command starts the profiling process, which will continue until stopped
- In MATLAB, run some code that calls the mex file
- Back in the VS command prompt

  - Call `VSDiagnostics.exe stop <SessionID> /output:<outputFile>`
    - The `<SessionID>` is the same as used to start the session
    - The `<outputFile>` is the name of a file where you want to store the
      output information. Do not provide a file extension. It will append
      the .diagsession extension.
  - Launch `devenv.exe <outputFile>.diagsession`
  - This is a polling type profiler. So at a fixed frequency, it will tell you what function is being executed. Inline functions will be excluded. You'll need to pick some code that takes at lease several seconds to run in order to get a meaningful result.
  - There will be a lot of "overhead" due to MATLAB. To filter this out, select the top level MEX function call.
    - For a C++ mex function this will be MexFunction::operator()
    - Could get confusing if you're calling multiple MEX functions

- for more info: https://learn.microsoft.com/en-us/visualstudio/profiling/profile-apps-from-command-line?view=vs-2022
