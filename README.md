# L3 Central event Muon Efficiency Study
L3 muon study includes HLT (+L1) re-emulation, Forest event matching and plotting efficiency & fake ratio for particular moudules to stuy for.
List of module studied is documantated in https://docs.google.com/spreadsheets/d/1_SR4VfEEuCVVaQ_XL0oYuynFt5iazNhk4DYlwV5xxso/edit#gid=1181379807

0.(LXP). Before re-emulation make a L3 Muon paths to test with.

1.(LXP). Like in "L3MuDataMenu_v3_L2fix.py" make the usual configuration file with RAWAODSIM output module and keep selected module outputs. (FEDRawDataCollection is too big and irrelevent so is dropped).

2.(T2). Run the Re-emulation crab jobs and refine track data in RAW with EDAnalyzer "L3PAnalyzer.cc".

------------------------------below codes should run with ACLiC with filename string(without '.root')------------------------------

3.(LOCAL). Note that the output is now offline reconstructed muons but online candidates, so through event by event matching this consist of several jobs
    + It is too heavy to run matching (~800M) * (~800M) events, so with match the event entry with "matchEvt_map.cc", this will provide map to map matching (~O(log^2)) for the same event number of online vs. reco and output text file of tree entry integer pair.
    + The "plotEffMuCent_v5.cc" refer to the event number and produce <ins>muon matching RECO efficiency, and online FAKE ratio plots.</ins>
 
4.(LOCAL). Modify graphs with "modify_Ratio_graph.cc" (<ins>Current status</ins>)
