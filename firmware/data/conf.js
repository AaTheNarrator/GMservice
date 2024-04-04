document.querySelector("#set-work-mode").onclick = () => {
    fetch('/api/v1/'+document.querySelector("#choosen-mode").value).then();
}
let info = {};

let update = () => {
    fetch("/api/v1/sys-info")
    .then((response) => response.json())
    .then((_info) => {
        info = _info;
        console.log(_info);
        document.querySelector("#mode").innerHTML = ["Пауза", "Работа", "Чтение"][_info.mode];
        document.querySelector("#temp").innerHTML = _info.temp;
        document.querySelector("#long").innerHTML = _info.long;
        document.querySelector("#lat").innerHTML = _info.lat;
        document.querySelector("#alt").innerHTML = _info.alt;
        document.querySelector("#sats").innerHTML = _info.sats;
        document.querySelector("#press").innerHTML = _info.pressure;
	    document.querySelector("#batt").innerHTML = _info.batt;
        document.querySelector("#time").innerHTML = _info.time;
        document.querySelector("#date").innerHTML = _info.date;
        document.querySelector("#L").innerHTML = `"Порог" = `+_info.l;
        document.querySelector("#M").innerHTML = `"Сдвиг" = `+_info.m;
        document.querySelector("#N").innerHTML = `"Усиление" = `+_info.n;
        document.querySelector("#count_cadr").innerHTML = _info.count_cadr;
    });
}
setInterval(update, 1000);


document.querySelector("#set-n").onclick = () => {
    fetch('/api/v1/N?value='+document.querySelector("#new-n").value).then(update);
}
document.querySelector("#set-m").onclick = () => {
    fetch('/api/v1/M?value='+document.querySelector("#new-m").value).then(update);
}
document.querySelector("#set-l").onclick = () => {
    fetch('/api/v1/L?value='+document.querySelector("#new-l").value).then(update);
}

//document.querySelector("#set-l").onclick = () => {
//    fetch('/api/v1/stabiliza').then(update);
//}
