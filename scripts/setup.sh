sudo rm -rf vpp
sudo rm dpdk-17.02.1.tar.xz
sudo rm -rf dpdk-17.02.1
sudo rm -rf results
sudo rm -rf plots
git clone https://github.com/vamsiDT/vpp.git
wget https://fast.dpdk.org/rel/dpdk-17.02.1.tar.xz
tar -xf dpdk-17.02.1.tar.xz
sudo rm -rf dpdk-17.02.1.tar.xz
wget https://git.dpdk.org/apps/pktgen-dpdk/snapshot/pktgen-3.1.2.tar.xz
tar -xf pktgen-3.1.2.tar.xz
sudo rm -rf pktgen-3.1.2.tar.xz
git clone https://github.com/TeamRossi/vpp_fairdrop.git
cd vpp_fairdrop
git checkout -f FlowMon
cd ./../
mv ./vpp_fairdrop ./FlowMon
cd $FLOW
make
cd $EXP
mkdir results
mkdir plots
