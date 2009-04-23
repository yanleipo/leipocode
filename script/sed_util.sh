#!/bash

echo "India's milk is good.
tea Red-Lable is good.
tea is better than the coffee." > teaormilk.txt

TEA=tea
MILK=milk

sed -i "s/$TEA/$MILK/g" teaormilk.txt

cat teaormilk.txt
rm teaormilk.txt -f
