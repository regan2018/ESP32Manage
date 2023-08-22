// pages/add/addBluetoothDevices.js
Page({

    /**
     * 页面的初始数据
     */
    data: {
        deviceList:[],
        isSearch:false,//是否在搜索
    },

    /**
     * 生命周期函数--监听页面加载
     */
    onLoad(options) {
        this.checkBluetoothOpenState(function(){
            wx.showToast({
              title: '已打开蓝牙',
            })
        });
    },

    /**
     * 生命周期函数--监听页面初次渲染完成
     */
    onReady() {

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
        this.stopBluetoothSearch();
    },
    //搜索蓝牙
    searchBluetooth(){
        let that=this;
        that.setData({isSearch:true});
        // 初始化蓝牙模块
        wx.openBluetoothAdapter({
            success: function(res) {
            // 开始扫描附近的设备
            wx.startBluetoothDevicesDiscovery({
                services: [],
                allowDuplicatesKey: false,
                success: function(res) {
                // 扫描到新设备时触发方法
                wx.onBluetoothDeviceFound(function(res2) {
                    console.log(res2)
                    that.setData({deviceList:res2.devices});
                })
                }
            })
            }
        });
    },
    // 停止扫描
    stopBluetoothSearch(){
        this.setData({isSearch:false});
        // 停止扫描
        wx.stopBluetoothDevicesDiscovery();
    },
    /**
     * 连接设备
     * @param {*} deviceId 
     */
    connectedDevice(deviceId){
        // 连接设备
        wx.createBLEConnection({
            deviceId: deviceId,
            success: function(res) {
            // 获取设备的服务列表
            wx.getBLEDeviceServices({
                deviceId: deviceId,
                success: function(res){
                for(var i = 0; i < res.services.length; i++){
                    if(res.services[i].uuid == "1234"){
                    // 获取设备的指定服务
                    wx.getBLEDeviceCharacteristics({
                        deviceId: deviceId,
                        serviceId: res.services[i].uuid,
                        success: function(res){
                        for(var j = 0; j < res.characteristics.length; j++){
                            if(res.characteristics[j].uuid == "4567"){
                            // 监听从设备传输过来的数据
                            wx.onBLECharacteristicValueChange(function(res){
                                var dataView = new DataView(res.value.buffer);
                                var value = dataView.getUint8(0);
                                console.log(value);
                            });
                            // 向设备发送数据
                            var buffer = new ArrayBuffer(1);
                            var dataView = new DataView(buffer);
                            dataView.setUint8(0, 1);

                            wx.writeBLECharacteristicValue({
                                deviceId: deviceId,
                                serviceId: res.services[i].uuid,
                                characteristicId: res.characteristics[j].uuid,
                                value: buffer,
                                success: function(res){
                                console.log(res.errMsg);
                                }
                            });
                            }
                        }
                        }
                    });
                    break;
                    }
                }
                }
            });
            }
        });
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
    
  // ListTouch触摸开始
  ListTouchStart(e) {
    this.setData({
      ListTouchStart: e.touches[0].pageX
    })
  },

  // ListTouch计算方向
  ListTouchMove(e) {
    this.setData({
      ListTouchDirection: e.touches[0].pageX - this.data.ListTouchStart > 0 ? 'right' : 'left'
    })
  },

  // ListTouch计算滚动
  ListTouchEnd(e) {
    if (this.data.ListTouchDirection =='left'){
      this.setData({
        modalName: e.currentTarget.dataset.target
      })
    } else {
      this.setData({
        modalName: null
      })
    }
    this.setData({
      ListTouchDirection: null
    })
  },
})