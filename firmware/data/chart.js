let E = [];
for (i = 0; i < 4096; i++) {
  E.push([i * 0.8193 - 215, null]);
}
E[2044] = [2044 * 0.8193 - 215, 10];

let fulldata = [];

var ctx = document.getElementById('myChart').getContext('2d');
let myChart = new Chart(ctx, {
  type: 'line',
  data: {
    datasets: [{
      pointBorderColor: 'transparent',
      pointBackgroundColor: 'transparent',
      label: 'Данные',
      borderColor: 'rgba(75, 192, 192, 1)',
      borderWidth: 1,
      fill: false
    }]
  },
  options: {
    scales: {
      x: {
        type: "linear", //"logarithmic",
        position: 'bottom',
        min: 1,
        max: 4096,
      },
      x2: {
        type: "linear",
        position: 'top',
        min: -215,
        max: 3116,
        grid:{
          display:false
        }
      },
      y: {
        type: 'linear',
        position: 'left',
        min: 0,
      }
    }
  },
  plugins: [{
    afterDraw: (chart, newSize) => {
      let ctx = chart.ctx;
      drawVerticalLine(ctx, chart, 2044, 'red');
      drawVerticalLine(ctx, chart, 3460, 'red');
    },
  }]
});

function drawVerticalLine(ctx, chart, xValue, color) {
  var xPixel = chart.scales.x.getPixelForValue(xValue);
  var chartArea = chart.chartArea;

  xPixel = Math.max(chartArea.left, Math.min(chartArea.right, xPixel));

  ctx.beginPath();
  ctx.moveTo(xPixel, chartArea.top);
  ctx.lineTo(xPixel, chartArea.bottom);
  ctx.strokeStyle = color;
  ctx.stroke();
  ctx.closePath();
}

let address = "/api/v1/acc";

async function updateChart() {
  fulldata = []
  for(let i = 0; i <= 40; i++){
    await fetch("http://192.168.4.1/api/v1/getAccData?num=" + i)
      .then(response => response.json())
      .then(data=>{
        console.log(data)
        fulldata.push(...data)
      })
  }
  console.log(fulldata)
  myChart.data.labels = Array.from({ length: fulldata.length }, (_, i) => i);
  myChart.data.datasets[0].data = fulldata;
  myChart.update();
  // fetch(address)
  //   .then(response => response.json())
  //   .then(data => {
  //     console.log(data);
  //     myChart.data.labels = Array.from({ length: data.length }, (_, i) => i);
  //     myChart.data.datasets[0].data = data;
  //     myChart.update();
  //   }).catch(error => console.error('Error:', error));;
}

setInterval(updateChart, 10000);

document.querySelector('#acc').addEventListener('change', (e) => {
  address = "/api/v1/acc";
});

document.querySelector('#cadr').addEventListener('change', (e) => {
  address = "/api/v1/cadr";
});

document.querySelector('#avg').addEventListener('change', (e) => {
  address = "/api/v1/avg";
});

document.querySelector('#log').addEventListener('change', (e) => {
  myChart.options.scales.x.type = 'logarithmic'
});

document.querySelector('#casual').addEventListener('change', (e) => {
  myChart.options.scales.x.type = 'linear'
});

document.querySelector('#clearBtn').addEventListener('click',()=>{
  fetch('/api/v1/clearGraphics').then(alert('Deleted'))
})

