# Create a new CSV file
rm output.csv
touch output.csv

# Init with new cols
echo 'student_id','score' > output.csv

# Recv the arguments and set the max_score and max_stdnt_id
numberOfArguments=$#

if [ $numberOfArguments -eq 0 ]; then
    max_score=100
    max_student_id=5
elif [ $numberOfArguments -eq 1 ]; then 
    max_score=$1
    max_student_id=5
else 
    max_score=$1
    max_student_id=$2
fi

last_student_id=`expr 1805120 + $max_student_id`

for ((i=1805121;i<=last_student_id;i++))
do 
    # Check if submitted or not
    if [ -e Submissions/$i/$i.sh ]; then
        for ((j=1805121;j<=last_student_id;j++)) 
        do 
            if [ "$i" != "$j" ]; then
                copycheckflag=0
                if [ -e Submissions/$j/$j.sh ]; then
                    copy_check=`diff -Z -B Submissions/$i/$i.sh Submissions/$j/$j.sh`
                    if [ ${#copy_check} -eq 0 ]; then 
                        copycheckflag=1
                        break
                    fi
                fi
            fi
        done 
        if [ $copycheckflag -eq 1 ]; then 
            echo $i,"-$max_score" >> output.csv 
        else   
            count=0
            ./Submissions/$i/$i.sh > temporary_file_1.txt
            diff -w AcceptedOutput.txt temporary_file_1.txt > temporary_file_2.txt
            while read p; do 
                if [ "${p:0:1}" = "<" ]; then
                    count=`expr $count + 1`
                elif [ "${p:0:1}" = ">" ]; then
                    count=`expr $count + 1`
                fi
            done < temporary_file_2.txt

            minus=`expr $count \* 5`

            if [ $minus -gt $max_score ]; then
                student_score=0
            else 
                student_score=`expr $max_score - $minus`
            fi
            echo $i,$student_score >> output.csv
        fi
    else
        # Insert 0 for assignment
        echo $i,'0' >> output.csv
        copycheckflag=0
    fi

done

rm temporary_file_1.txt
rm temporary_file_2.txt