rm -rf data
mkdir data

# стили
cp ../ui/index.css               data

# График
cp ../ui/index.html              data
#cp ../ui/jquery.flot.min.js data
#cp ../ui/jquery.min.js      data
cp ../ui/jquery.chart.js    data
cp ../ui/chart.js                data

# Настройка
cp ../ui/conf.html               data
cp ../ui/conf.js                 data

pio run --target buildfs
