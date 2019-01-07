##
# Converts a series of csv files into images and a gif
# It makes use of the image_builder executable


#where the csv files are located
exp_path=/home/jean/Pictures/ir/test/

#what number is the palette type?
palette_num=10

#how frequent is each snapshot taken
frequency=10

#where is the csv to ppm executable located 
project_dir=`pwd`
csv_to_ppm="$project_dir/build/image_builder"

mkdir -p $exp_path
cd $exp_path

mkdir -p png
mkdir -p ppm
mkdir -p data
mkdir -p png/$palette_num

ts=0 #timestamp

for f in *.csv; do
	# echo "$f $exp_path $exp_path"

	eval "$csv_to_ppm $exp_path $f"

	#convert each ppm to png and fix transformations
	for i in *.ppm; do
		out="${i%.ppm}.png"
		convert $i $out #convert to png
		convert $i -flip $out #un-mirror the image
		convert -rotate 90 $out $out 
		convert $out -gravity Southwest -pointsize 18 stroke '#000C' -strokewidth 2 -annotate 0 $ts $out
	done


	#add timestamp to bottom on png 
	mv $f data
	mv *.ppm ppm
	mv *"$palette_num".png png/$palette_num
	ts=$((ts + $frequency))
	echo

done

cd $exp_path
cd png/$palette_num


#create gif from all images
convert -loop 0 -delay 5 -dispose background *.png out.gif 
	
