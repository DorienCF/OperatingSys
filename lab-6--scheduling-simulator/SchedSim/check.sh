rm schedsim
make schedsim
./schedsim input0.txt > out1
./schedsim_ref input0.txt > out2
diff out1 out2
