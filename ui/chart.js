let fulldata = [];
let update = true;

var ctx = document.getElementById('myChart').getContext('2d');
let myChart = new Chart(ctx, {
  type: 'line',
  data: {
    datasets: [{
      pointBorderColor: 'transparent',
      pointBackgroundColor: 'transparent',
      label: 'Данные',
      borderColor: 'rgba(230, 215, 0, 1)',
      borderWidth: 1,
      fill: false
    }]
  },
  options: {
    responsive: true,
    maintainAspectRatio: false,
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
    },
    animation: {
      duration: 0,
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

const server = "http://192.168.4.1";
let request = "/api/v1/acc"; 

async function updateChart() {
  fulldata = [];
  const maxWaitTime = 200; // 3 секунды

  for (let i = 0; i < 4; i++) {
    let startTime = Date.now();

    await fetch(server + request + "?num=" + i)
      .then(async (response) => {
        try {
          let data = await response.json();
          console.log(data);
          fulldata.push(...data);
        } catch (error) {
          console.log("Error parsing JSON:", error);
        }
      })
      .catch((error) => {
        console.log("Fetch error:", error);
      });

    let elapsedTime = Date.now() - startTime;
    
    if (elapsedTime < maxWaitTime) {
      await sleep(maxWaitTime - elapsedTime);
    }
  }

  console.log(fulldata);
  myChart.data.labels = Array.from({ length: fulldata.length }, (_, i) => i);
  myChart.data.datasets[0].data = fulldata;
  myChart.update();
  await updateChart();
}

function sleep(ms) {
  return new Promise((resolve) => setTimeout(resolve, ms));
}

updateChart();

async function getCountCadr(){
  await fetch(server + "/api/v1/countCadr")
    .then(response => response.json())
    .then(data => document.querySelector('#count_cadr').innerHTML = data)
}

setInterval(getCountCadr, 1000)

function setCanvasSize() {
  if (window.innerWidth > 500) return;
  const canvas = document.getElementById('myChart');
  const aspectRatio = 9 / 16; // Пример: соотношение сторон 16:9
  const maxWidth = window.innerWidth; // Максимальная ширина экрана

  // Устанавливаем ширину и высоту canvas в зависимости от ширины экрана и соотношения сторон
  canvas.width = maxWidth;
  canvas.height = maxWidth / aspectRatio;
}

window.addEventListener('load', setCanvasSize);

document.querySelector('#acc').addEventListener('change', (e) => {
  request = "/api/v1/acc";
});

document.querySelector('#cadr').addEventListener('change', (e) => {
  request = "/api/v1/cadr";
});

document.querySelector('#avg').addEventListener('change', (e) => {
  request = "/api/v1/avg";
});

document.querySelector('#log').addEventListener('change', (e) => {
  myChart.options.scales.x.type = 'logarithmic'
});

document.querySelector('#casual').addEventListener('change', (e) => {
  myChart.options.scales.x.type = 'linear'
});

document.querySelector('#clearBtn').addEventListener('click',()=>{
  if (confirm("Удалить график?")){
    fetch(server + '/api/v1/clearGraphics').then(alert('Deleted'))
  }
})

document.querySelector('#openSettings').addEventListener('click',()=>{
  update = false
  window.location.href = "/conf.html"
})