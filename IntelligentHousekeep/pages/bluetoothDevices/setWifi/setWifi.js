// pages/bluetoothDevices/setWifi/setWifi.js
import {
    formatTime,
    ab2hex,
    hexToString,
    str2ab,
    uuid2Mac,
    inArray
} from '../../../utils/util';

Page({

    /**
     * 页面的初始数据
     */
    data: {
        connectBTEStatus:false,//蓝牙连接状态
        servicesUUID:"",
        characteristicUUID:"",
        isShowPwd:false,//是否显示密码框
        wifiList:[],
        isSearch:false,//是否在搜索
        deviceId:'',//蓝牙设备ID
        deviceName:'',//蓝牙设备名称
        wifiName:'',//wifi名称
        wifiPwd:''//输入的wifi密码
    },

    /**
     * 生命周期函数--监听页面加载
     */
    onLoad(options) {
        console.log(options)
        this.setData({deviceId:options.deviceId,deviceName:options.deviceName});
        let that=this;
        this.checkPhoneSupportWifi(function(){
            wx.showToast({
              title: '支持使用WIFI',
            })
            that.searchWifi();
        });
    },

    /**
     * 生命周期函数--监听页面初次渲染完成
     */
    onReady() {

    },
    showPwdModal(e){
        var index=e.currentTarget.dataset.index;
        var wifiName=this.data.wifiList[index].SSID;
        this.setData({isShowPwd:true,wifiName:wifiName})
    },
    hidePwdModal(){
        this.setData({isShowPwd:false});
    },
    searchWifi(){
        let that=this;
        this.setData({isSearch:true})
        wx.startWifi({
            success:function(res){
                console.log(res);
                wx.getWifiList({
                    success:function(res2){
                        console.log("getWifiList：",res2)
                        wx.onGetWifiList((result) => {
                            console.log("onGetWifiList：",result);
                            that.setData({wifiList:result.wifiList})
                        });
                    },fail:function(res2){
                        console.log(res2);
                        wx.showToast({
                          title: '获取WIFI列表失败',
                          icon:"error"
                        })
                    }

                });
               
            },
            fail:function(res){
                wx.showToast({
                  title: '初始化WIFI失败',
                  icon:"error"
                })
            }
        });
    },
    restartWifi(){
        this.setData({deviceList:[]});
        this.stopWifiSearch();
        this.searchWifi();
    },
    stopWifiSearch(){
        this.setData({isSearch:false});
        wx.stopWifi();
    },
    inputPwd(e){
        const pwd = e.detail.value;
        this.setData({wifiPwd:pwd})
        console.log(pwd);
    },
    /**
     * 生命周期函数--监听页面显示
     */
    onShow() {

    },

    /**
     * 生命周期函数--监听页面隐藏
     */
    onHide() {

    },

    /**
     * 生命周期函数--监听页面卸载
     */
    onUnload() {
        this.stopBLEDevicesTask();
        wx.stopWifi();
    },

    /**
     * 页面相关事件处理函数--监听用户下拉动作
     */
    onPullDownRefresh() {

    },

    /**
     * 页面上拉触底事件的处理函数
     */
    onReachBottom() {

    },

    /**
     * 用户点击右上角分享
     */
    onShareAppMessage() {

    },
     //检测手机型号是否支持WIFI
    checkPhoneSupportWifi(callback){
        wx.getSystemInfo({
            success: function(res) {
                var system = '';
                if (res.platform == 'android') system = parseInt(res.system.substr(8));
                if (res.platform == 'ios') system = parseInt(res.system.substr(4));
                if (res.platform == 'android' && system < 6) {
                    wx.showToast({
                        title: '手机版本不支持',
                        icon:"error"
                    })
                    return
                }
                if (res.platform == 'ios' && system < 11.2) {
                    wx.showToast({
                        title: '手机版本不支持',
                        icon:"error"
                    })
                    return
                }
                if (callback) {// 这里是回调方法
                    callback() // 这里是回调方法
                }
            }
        })
    },
    //发送wifi配置信息到设备
    sendWifiCfgInfo(){
        this.hidePwdModal();
        var that=this;
        var wifiName=this.data.wifiName;
        var wifiPwd=this.data.wifiPwd;
        var deviceId=this.data.deviceId;
        var servicesUUID=this.data.servicesUUID;
        var characteristicUUID=this.data.characteristicUUID;
        wx.showLoading({
            title: '检测系统蓝牙是否已打开……',
        })

        if(this.data.connectBTEStatus){
            //已连接蓝牙
            //向蓝牙发送指令
            that.sendWifiBLEData(wifiName,wifiPwd,deviceId,servicesUUID,characteristicUUID);
        }else{
            //未连接蓝牙
            this.checkBluetoothOpenState(function(){
                //连接设备
                that.connectedDevice(wifiName,wifiPwd);
            });
        }
    },
    /**
     * 连接设备并发送wifi连接信息
     */
    connectedDevice(wifiName,wifiPwd){
        let that=this;
        var deviceId=this.data.deviceId;
        wx.showLoading({
          title: '正在连接蓝牙……',
        })
         // 初始化蓝牙模块
         wx.openBluetoothAdapter({
            success: function(res) {
                // 连接设备
                wx.createBLEConnection({
                    deviceId: deviceId,
                    success: function(res) {
                        wx.showLoading({
                        title: '正在获取设备服务列表',
                        })
                    // 获取设备的服务列表
                    wx.getBLEDeviceServices({
                        deviceId: deviceId,
                        success: function(res2){
                            console.log("获取设备的服务列表：",res2)
                            for(var i = 0; i < res2.services.length; i++){
                                var servicesUUID=res2.services[i].uuid;
                                if(servicesUUID.toLowerCase() == "4fafc201-1fb5-459e-8fcc-c5c9c331914b"){
                                    wx.showLoading({
                                        title: '正在获取设备的指定服务',
                                    })
                                    // 获取设备的指定服务
                                    wx.getBLEDeviceCharacteristics({
                                        deviceId: deviceId,
                                        serviceId: servicesUUID,
                                        success: function(res3){
                                            for(var j = 0; j < res3.characteristics.length; j++){
                                                var characteristicUUID=res3.characteristics[j].uuid;
                                                if(characteristicUUID.toLowerCase() == "beb5483e-36e1-4688-b7f5-ea07361b26a8"){
                                                    wx.showLoading({
                                                        title: '数据发送中……',
                                                    })
                                                    that.setData({servicesUUID:servicesUUID,characteristicUUID:characteristicUUID});
                                                    //检测蓝牙连接状态
                                                    that.BLEConnectionStateChange();
                                                    // 监听从设备传输过来的数据
                                                    that.BLECharacteristicValueChange()
                                                    // wx.onBLECharacteristicValueChange(function(res4){
                                                    //     var dataView = new DataView(res4.value.buffer);
                                                    //     var value = dataView.getUint8(0);
                                                    //     console.log(value);
                                                    // });
                                                    
                                                    //向蓝牙发送指令
                                                    that.sendWifiBLEData(wifiName,wifiPwd,deviceId,servicesUUID,characteristicUUID);
                                                    
                                                }
                                            }
                                        }
                                    });
                                    break;
                                }
                            }
                        },
                        fail:function(res2){
                            wx.hideLoading();
                            wx.showToast({
                                title: '获取设备服务列表',
                                icon:"error"
                            })
                        }
                    });
                    },
                    fail:function(res){
                        wx.hideLoading();
                        wx.showToast({
                        title: '蓝牙连接失败',
                        icon:"error"
                        })
                    }
                });
            }
        });
    },
    /**
   * 监听蓝牙数据
   * @param
   */
  BLECharacteristicValueChange() {
      let that=this;
        wx.onBLECharacteristicValueChange((characteristic) => {
            let { characteristicId, value } = characteristic;
            // const idx = inArray(this.characteristicStack, "uuid", characteristicId);
            let formatedValue = hexToString(ab2hex(value));
            console.log("蓝牙接收到的值：",formatedValue)
        });
    },
    /**
     * 发送蓝牙数据
     * @param {*} wifiName 
     * @param {*} wifiPwd 
     * @param {*} deviceId 
     * @param {*} servicesUUID 
     * @param {*} characteristicUUID 
     */
    sendWifiBLEData(wifiName,wifiPwd,deviceId,servicesUUID,characteristicUUID){
        var sendData={
            "wifiName":wifiName,
            "wifiPwd":wifiPwd
        };
        console.log("发送的数据：",wifiName,wifiPwd,deviceId,servicesUUID,characteristicUUID)
        //向蓝牙发送指令
        this.writeBLEValue(deviceId,servicesUUID,characteristicUUID,sendData,"setWifi");
    },

    /**
     * 发送蓝牙指令。蓝牙指令超出20字符时需要截断多次发送
     * @param {string} cmdStr 蓝牙指令
     * @param {string} cmdName 蓝牙指令名称——可选用于打印调试
     */
    writeBLEValue(deviceId,serviceId,characteristicId,cmdStr, cmdName) {
        let that=this;
        console.log("发送蓝牙指令------------》", cmdStr, cmdName);

        var sendData={
            cmdName:cmdName,
            cmdVal:cmdStr
        };
        
        let buffer = str2ab(JSON.stringify(sendData));
        let param = {
            deviceId: deviceId,
            serviceId: serviceId,
            characteristicId: characteristicId,
            value: buffer,
            writeType:"write"
        };
        console.log("发送指令"+cmdName+"：", param);
        wx.writeBLECharacteristicValue({
            ...param,
            success: function (res) {
                wx.hideLoading();
                wx.showToast({
                  title: '发送指令成功',
                  icon:'success'
                })
                console.log("发送指令成功", cmdName);
                //that.closeBLEConnection(deviceId);
            },
            fail: function (res) {
                wx.showToast({
                    title: '发送指令失败',
                    icon:'error'
                  })
                console.warn("发送指令失败", cmdName, res);
                that.closeBLEConnection(deviceId);
            },
        });
    },
    /**
     * 分段发送长数据
     * @param {*} deviceId 
     * @param {*} serviceId 
     * @param {*} characteristicId 
     * @param {string} cmdStr 蓝牙指令
     * @param {string} cmdName 蓝牙指令名称——可选用于打印调试 
     */
    subWriteBLEValue(deviceId,serviceId,characteristicId,cmdStr, cmdName){
        let that=this;
        var sendData={
            cmdName:cmdName,
            cmdVal:cmdStr
        };
        console.log("发送蓝牙指令------------》", cmdStr, cmdName);
        sendData=JSON.stringify(sendData);
        var byteLen = sendData.length;
        var pos = 0;
        let loopCount = 0;
        // 消息超长分批处理
        for (let i = 0; i < byteLen; i += 20) {
            let buffer = str2ab(sendData.slice(pos, pos + 20));
            pos += 20;
            loopCount += 1;
            let param = {
                deviceId: deviceId,
                serviceId: serviceId,
                characteristicId: characteristicId,
                value: buffer,
                writeType:"write"
            };
            console.log("第"+loopCount+"次发送指令"+cmdName+"：", param);
            wx.writeBLECharacteristicValue({
                ...param,
                success: function (res) {
                    console.log("发送指令成功", cmdName);
                },
                fail: function (res) {
                    console.warn("发送指令失败", cmdName, res);
                    that.closeBLEConnection(deviceId);
                },
            });
        }
        //that.closeBLEConnection(deviceId);
    },
    /**
   * 3.6 蓝牙状态变化监听
   */
  BLEConnectionStateChange() {
      let that=this;
        wx.onBLEConnectionStateChange((res) => {
            let { deviceId, connected } = res;
            that.setData({connectBTEStatus: connected});
            console.log("蓝牙状态变化 -------------》", connected, deviceId);
        });
    },
     /**
     * 停止蓝牙通信活动及监听
     */
    stopBLEDevicesTask() {
        // 停止扫描设备
        wx.stopBluetoothDevicesDiscovery();
        // 关闭扫描新设备监听
        wx.offBluetoothDeviceFound();
        // 关闭数据监听
        wx.offBLECharacteristicValueChange();
        // 移除蓝牙低功耗连接状态改变事件的监听函数
        wx.offBLEConnectionStateChange();
        wx.closeBluetoothAdapter({
            success:function(res){

            },
            complete:function(res){
                console.log("关闭蓝牙模块：",res)
            }
        });
    },
    //关闭蓝牙的设备连接
    closeBLEConnection(deviceId){
        wx.closeBLEConnection({
            deviceId:deviceId,
            success:function(res2){
                console.log("关闭连接成功：",res2)
            },fail:function(res2){
                console.log("关闭连接失败：",res2)
            }
        })
    },
    /**
     * 检查蓝牙是否打开
     * @param {} callback 
     */
    checkBluetoothOpenState(callback){
        wx.getSystemInfo({
            success: function (res) {
              var res = wx.getSystemInfoSync();
              if (!res.bluetoothEnabled && !res.locationEnabled) {
                wx.hideLoading();
                wx.showModal({
                  title: '提示',
                  content: '您的蓝牙和GPS未开启,请先开启蓝牙和GPS',
                  showCancel: false,
                  success: function () {
                    wx.navigateBack({
                      delta: 1,
                    })
                  }
                })
              } else if (!res.bluetoothEnabled) {
                wx.hideLoading();
                wx.showModal({
                  title: '提示',
                  content: '您的蓝牙未开启,请先开启蓝牙',
                  showCancel: false,
                  success: function () {
                    wx.navigateBack({
                      delta: 1,
                    })
                  }
                })
              } else if (!res.locationEnabled) {
                wx.hideLoading();
                wx.showModal({
                  title: '提示',
                  content: '您的GPS未开启,请先开启GPS',
                  showCancel: false,
                  success: function () {
                    wx.navigateBack({
                      delta: 1,
                    })
                  }
                })
              } else if (res.bluetoothEnabled && res.locationEnabled) {
                if (callback) {// 这里是回调方法
                  callback() // 这里是回调方法
                }
              }
            },
          })      
    },
})