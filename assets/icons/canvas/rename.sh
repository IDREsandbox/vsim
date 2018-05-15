

#ls * | sed -e 'p;s/base/START/' | xargs -n2 mv

olds=$(find baseline*)
for old in $olds; do
    new=$(echo $old | sed -e 's/baseline_\(.*\)_white_18dp.png/\1.png/')
    # echo "test"
    #echo "old $old"
    #echo "new $new"
    mv $old $new
done
# ls * | sed -e 'p;s/baseline_\(.*\)_white_18dp.png/\1.png/' | cat