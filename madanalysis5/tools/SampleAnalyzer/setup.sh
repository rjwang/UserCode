#!/bin/sh

# Defining colours for shell
GREEN="\\033[1;32m"
RED="\\033[1;31m"
PINK="\\033[1;35m"
BLUE="\\033[1;34m"
YELLOW="\\033[1;33m"
CYAN="\\033[1;36m"
NORMAL="\\033[0;39m"

# Configuring PATH environment variable
if [ $PATH ]; then
export PATH=$PATH:/Users/Renjie/work_invHiggs/MadGraph_Study/MadAnalysis5/madanalysis5/tools/fastjet/bin
else
export PATH=/Users/Renjie/work_invHiggs/MadGraph_Study/MadAnalysis5/madanalysis5/tools/fastjet/bin
fi

# Configuring LD_LIBRARY_PATH environment variable
if [ $LD_LIBRARY_PATH ]; then
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/Users/Renjie/work_invHiggs/MadGraph_Study/MadAnalysis5/madanalysis5/tools/SampleAnalyzer/Lib:/usr/local/lib/root:/Users/Renjie/work_invHiggs/MadGraph_Study/MadAnalysis5/madanalysis5/tools/fastjet/lib:/usr/lib:/Users/Renjie/work_invHiggs/MadGraph_Study/MadAnalysis5/madanalysis5/tools/delphes
else
export LD_LIBRARY_PATH=/Users/Renjie/work_invHiggs/MadGraph_Study/MadAnalysis5/madanalysis5/tools/SampleAnalyzer/Lib:/usr/local/lib/root:/Users/Renjie/work_invHiggs/MadGraph_Study/MadAnalysis5/madanalysis5/tools/fastjet/lib:/usr/lib:/Users/Renjie/work_invHiggs/MadGraph_Study/MadAnalysis5/madanalysis5/tools/delphes
fi

# Configuring DYLD_LIBRARY_PATH environment variable
if [ $DYLD_LIBRARY_PATH ]; then
export DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH:/Users/Renjie/work_invHiggs/MadGraph_Study/MadAnalysis5/madanalysis5/tools/SampleAnalyzer/Lib:/usr/local/lib/root:/Users/Renjie/work_invHiggs/MadGraph_Study/MadAnalysis5/madanalysis5/tools/fastjet/lib:/usr/lib:/Users/Renjie/work_invHiggs/MadGraph_Study/MadAnalysis5/madanalysis5/tools/delphes
else
export DYLD_LIBRARY_PATH=/Users/Renjie/work_invHiggs/MadGraph_Study/MadAnalysis5/madanalysis5/tools/SampleAnalyzer/Lib:/usr/local/lib/root:/Users/Renjie/work_invHiggs/MadGraph_Study/MadAnalysis5/madanalysis5/tools/fastjet/lib:/usr/lib:/Users/Renjie/work_invHiggs/MadGraph_Study/MadAnalysis5/madanalysis5/tools/delphes
fi

# Checking that all environment variables are defined
if [[ $PATH && $LD_LIBRARY_PATH && $DYLD_LIBRARY_PATH ]]; then
echo -e $YELLOW"--------------------------------------------------------"
echo -e "    Your environment is properly configured for MA5     "
echo -e "--------------------------------------------------------"$NORMAL
fi
