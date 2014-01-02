BLOCK_SIZES="64 128"
CACHE_SIZES="8192 16384"
ASSOCIATIVITIES="4 8 16"
TRACES=`ls trace.*`

for b in $BLOCK_SIZES; do
  for s in $CACHE_SIZES; do
    for a in $ASSOCIATIVITIES; do
      for t in $TRACES; do
        echo -n "$t, $b, $s, $a, "
        ./cachesim $t $b $s $a
      done
    done  
  done
done
