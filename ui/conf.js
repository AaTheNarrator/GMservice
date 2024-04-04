const server = "http://192.168.4.1";

let info = {};
let downloading = false;
let stabmode = ["Откл","Вкл"];

let update = () => {
    //console.log(downloading);
    //if (downloading) return;

  fetch(server + '/api/v1/sys-info')
  .then((response) => response.json())
  .then((_info) => {
    info = _info;
    console.log(_info);
    document.querySelector('#mode').innerHTML = ['Пауза', 'Работа', 'Чтение'][_info.mode];
    document.querySelector('#temp').innerHTML = _info.temp;
    document.querySelector('#long').innerHTML = _info.long;
    document.querySelector('#lat').innerHTML = _info.lat;
    document.querySelector('#alt').innerHTML = _info.alt;
    document.querySelector('#sats').innerHTML = _info.sats;
    document.querySelector('#press').innerHTML = _info.pressure;
    document.querySelector('#batt').innerHTML = _info.batt;
    document.querySelector('#time').innerHTML = _info.time;
    document.querySelector('#date').innerHTML = _info.date;
    document.querySelector('#L').innerHTML = `'Порог' = `+_info.l;
    document.querySelector('#M').innerHTML = `'Сдвиг' = `+_info.m;
    document.querySelector('#N').innerHTML = `'Усиление' = `+_info.n;
    document.querySelector('#count_cadr').innerHTML = _info.count_cadr;
    document.querySelector('#stabmode').innerHTML = stabmode[_info.stabilization]
    document.querySelector('#stabmodebtn').innerHTML = stabmode[Number(!_info.stabilization)]
  });
}
setInterval(update, 1000);


document.querySelector('#set-n').onclick = () => {
  fetch(server + '/api/v1/N?value='+document.querySelector('#new-n').value).then(update);
}
document.querySelector('#set-m').onclick = () => {
  fetch(server + '/api/v1/M?value='+document.querySelector('#new-m').value).then(update);
}
document.querySelector('#set-l').onclick = () => {
  fetch(server + '/api/v1/L?value='+document.querySelector('#new-l').value).then(update);
}

document.querySelector('#set-work-mode').onclick = () => {
  fetch(server + '/api/v1/'+document.querySelector('#choosen-mode').value)//.then(downloading = false);
}

document.querySelector('#stabilize').addEventListener('click',()=>{
    if (!info.stabilization){
        fetch(server + "/api/v1/stabilization-on")
    }
    else{
        fetch(server + "/api/v1/stabilization-off")
    }
})

document.querySelector('#deleteBtn').addEventListener('click', ()=>{
    fetch(server + '/api/v1/delete').then(data => data.text()).then(text => alert(text))
})
