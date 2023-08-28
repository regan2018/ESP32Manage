const formatTime = date => {
  const year = date.getFullYear()
  const month = date.getMonth() + 1
  const day = date.getDate()
  const hour = date.getHours()
  const minute = date.getMinutes()
  const second = date.getSeconds()

  return `${[year, month, day].map(formatNumber).join('/')} ${[hour, minute, second].map(formatNumber).join(':')}`
}

const formatNumber = n => {
  n = n.toString()
  return n[1] ? n : `0${n}`
}

// 将字符串转为 ArrayBuffer
const str2ab = str => {
    var buf = new ArrayBuffer(str.length);
    var bufView = new Uint8Array(buf);
    for (var i = 0, strLen = str.length; i < strLen; i++) {
      bufView[i] = str.charCodeAt(i);
    }
    return buf;
  }

// ArrayBuffer转16进度字符串
const ab2hex = buffer => {
    var hexArr = Array.prototype.map.call(new Uint8Array(buffer), function (bit) {
      return ("00" + bit.toString(16)).slice(-2);
    });
    return hexArr.join("");
  }
  
  // 将16进制转为 字符串
  const hexToString = str => {
    var val = "",
      len = str.length / 2;
    for (var i = 0; i < len; i++) {
      val += String.fromCharCode(parseInt(str.substr(i * 2, 2), 16));
    }
    return utf8to16(val);
  }
  //uuid2Mac 统一安卓与IOS端deviceId展示
  const uuid2Mac = advertisData => {
    if (advertisData) {
      let bf = advertisData.slice(3, 9);
      let mac = Array.prototype.map
        .call(new Uint8Array(bf), (x) => ("00" + x.toString(16)).slice(-2))
        .join(":");
      mac = mac.toUpperCase();
      return mac;
    }
  }

  //新设备去重
  function inArray(arr, key, val){
    for (let i = 0; i < arr.length; i++) {
      if (arr[i][key] === val) {
        return i;
      }
    }
    return -1;
  }

module.exports = {
  formatTime,
  ab2hex,
  hexToString,
  str2ab,
  uuid2Mac,
  inArray
}
