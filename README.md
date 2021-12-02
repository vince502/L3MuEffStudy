# L3 Central event Muon Efficiency Study
L3 muon study includes HLT (+L1) re-emulation, Forest event matching and plotting efficiency & fake ratio for particular moudules to stuy for.
List of module studied is documantated in https://docs.google.com/spreadsheets/d/1_SR4VfEEuCVVaQ_XL0oYuynFt5iazNhk4DYlwV5xxso/edit#gid=1181379807

0.(Usually in LXPLUS). Before re-emulation make a L3 Muon paths to test with.

1.(Usually in LXPLUS). Like in "L3MuDataMenu_v3_L2fix.py" make the usual configuration file with RAWAODSIM output module and keep selected module outputs. (FEDRawDataCollection is too big and irrelevent so is dropped).

2.(T2). Run the Re-emulation crab jobs and refine track data in RAW with EDAnalyzer "L3PAnalyzer.cc".

3.(LOCAL). Note that the output is now offline reconstructed muons but online candidates, so through event by event matching this consist of several jobs
 
4.(LOCAL). Modify graphs with "modify_Ratio_graph.cc" (<ins>Current status</ins>)
