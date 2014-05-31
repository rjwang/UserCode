#!/bin/csh -f

# Defining colours for shell
set GREEN  = "\033[1;32m"
set RED    = "\033[1;31m"
set PINK   = "\033[1;35m"
set BLUE   = "\033[1;34m"
set YELLOW = "\033[1;33m"
set CYAN   = "\033[1;36m"
set NORMAL = "\033[0;39m"

# Configuring PATH environment variable
if ( $?PATH ) then
setenv PATH "$PATH":/Users/Renjie/work_invHiggs/MadGraph_Study/MadAnalysis5/madanalysis5/tools/fastjet/bin
else
setenv PATH /Users/Renjie/work_invHiggs/MadGraph_Study/MadAnalysis5/madanalysis5/tools/fastjet/bin
endif

# Configuring LD_LIBRARY_PATH environment variable
if ( $?LD_LIBRARY_PATH ) then
setenv LD_LIBRARY_PATH "$LD_LIBRARY_PATH":/Users/Renjie/work_invHiggs/MadGraph_Study/MadAnalysis5/madanalysis5/tools/SampleAnalyzer/Lib:/usr/local/lib/root:/Users/Renjie/work_invHiggs/MadGraph_Study/MadAnalysis5/madanalysis5/tools/fastjet/lib:/usr/lib:/Users/Renjie/work_invHiggs/MadGraph_Study/MadAnalysis5/madanalysis5/tools/delphes
else
setenv LD_LIBRARY_PATH /Users/Renjie/work_invHiggs/MadGraph_Study/MadAnalysis5/madanalysis5/tools/SampleAnalyzer/Lib:/usr/local/lib/root:/Users/Renjie/work_invHiggs/MadGraph_Study/MadAnalysis5/madanalysis5/tools/fastjet/lib:/usr/lib:/Users/Renjie/work_invHiggs/MadGraph_Study/MadAnalysis5/madanalysis5/tools/delphes
endif

# Configuring DYLD_LIBRARY_PATH environment variable
if ( $?DYLD_LIBRARY_PATH ) then
setenv DYLD_LIBRARY_PATH "$DYLD_LIBRARY_PATH":/Users/Renjie/work_invHiggs/MadGraph_Study/MadAnalysis5/madanalysis5/tools/SampleAnalyzer/Lib:/usr/local/lib/root:/Users/Renjie/work_invHiggs/MadGraph_Study/MadAnalysis5/madanalysis5/tools/fastjet/lib:/usr/lib:/Users/Renjie/work_invHiggs/MadGraph_Study/MadAnalysis5/madanalysis5/tools/delphes
else
setenv DYLD_LIBRARY_PATH /Users/Renjie/work_invHiggs/MadGraph_Study/MadAnalysis5/madanalysis5/tools/SampleAnalyzer/Lib:/usr/local/lib/root:/Users/Renjie/work_invHiggs/MadGraph_Study/MadAnalysis5/madanalysis5/tools/fastjet/lib:/usr/lib:/Users/Renjie/work_invHiggs/MadGraph_Study/MadAnalysis5/madanalysis5/tools/delphes
endif

# Checking that all environment variables are defined
if ( 
$?PATH && $?LD_LIBRARY_PATH && $?DYLD_LIBRARY_PATH ) then
echo $YELLOW"--------------------------------------------------------"
echo "    Your environment is properly configured for MA5     "
echo "--------------------------------------------------------"$NORMAL
endif
