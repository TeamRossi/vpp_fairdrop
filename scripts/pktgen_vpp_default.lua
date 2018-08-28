package.path = package.path ..";?.lua;test/?.lua;app/?.lua;"
--pktgen.start(0);
--just to make sure that there are no rx-miss and tx-error in VPP
--pktgen.delay(5000);
--pktgen.clear("all");
--pktgen.stop(0);

function printdata(averg, inp, outp, noloss, rate)
    printf("SIZE: %d IN: %d OUT: %d LOST: %d NOLOSS: %.8f RATE: %.2f\n", averg, inp, outp, (outp-inp), noloss, rate);
end

--outputp=0
--inputp=0
--noloss=0
--rate=0
SIZE=64
SECONDS=1000
SLEEP=20
RATE=100

local file = io.open ("/tmp/pktgenstats.dat", "w");
io.output(file);

--pktgen.start(0);
--pktgen.delay(10000);
--pktgen.stop(0);
pktgen.set("0","size",SIZE);
pktgen.set("0","rate",RATE);
--pktgen.set("0","count".200000000);

        --os.execute("/home/vk/scripts_cpu/runs.sh 'clear interfaces' ");
        --os.execute("/home/vk/scripts_cpu/runs.sh 'clear hardware' ");
--		os.execute("/home/vk/scripts_cpu/runs.sh 'clear run' ");
--os.execute("/home/vk/scripts_cpu/runs.sh 'clear run' ");
pktgen.delay(5000);
pktgen.start(0);
--pktgen.delay(20000);
--os.execute("/home/vk/scripts_cpu/runs.sh 'clear run' ");
pktgen.delay(SLEEP*SECONDS);
--os.execute("/home/vk/scripts_cpu/runs.sh 'show run' > /tmp/show ");
--os.execute("/home/vk/scripts_cpu/runs.sh 'show interface' > /tmp/data ");
outputp=(pktgen.portStats("all", "port")[0]["opackets"]);
inputp=0;
noloss=0;
--inputp=(pktgen.portStats("all", "port")[1]["ipackets"]);
--noloss= 1-(outputp-inputp)/outputp;
rate=inputp/(SLEEP*SECONDS);
printdata(SIZE, inputp, outputp, noloss, rate);
--os.execute("/home/vk/scripts_cpu/runs.sh 'show interface' > /tmp/data ");
--os.execute("/home/vk/scripts_cpu/runs.sh 'show run' > /tmp/show ");
pktgen.stop(0);
os.execute("sudo kill -2 $(pidof FlowMon-DPDK)");
io.close(file);
pktgen.delay(5000);
os.execute("echo DONE");
--os.execute("/home/vk/scripts_cpu/runs.sh 'event-logger save flowvqueue'");
--os.execute("cp /tmp/flowvqueue /home/vk/vpp_cpu_faidrop_experiments/flowvqueue");
--os.execute("python3 /home/vk/vpp_cpu_faidrop_experiments/binary_log_read.py /tmp/flowvqueue > /tmp/flowvqueue.distr.vec");
--os.execute("cp /tmp/flowvqueue.distr.vec /home/vk/vpp_cpu_faidrop_experiments/flowvqueue.distr.vec");

--pktgen.capture(1,"enable");
--pktgen.delay(1000);
--pktgen.stop(0);
--pktgen.delay(3000);
--pktgen.capture(1,"disable");
--pktgen.delay(5000);

os.execute("sudo kill -9 $(pidof pktgen)");
