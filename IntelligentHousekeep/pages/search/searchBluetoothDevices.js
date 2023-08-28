
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
            // wx.showToast({
            //   title: '已打开蓝牙',
            // })
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
                success: function(res1) {
                    console.log(res1);
                    // 扫描到新设备时触发方法
                    wx.onBluetoothDeviceFound(function(res2) {
                        console.log(res2)
                        var list=that.data.deviceList;
                        var tempList=res2.devices;
                        for(var i=0;i<tempList.length;i++){
                            var item=tempList[i];
                            list.push(item);
                        }
                        that.setData({deviceList:list});
                    })
                }
            })
            }
        });
    },
    //重新搜索
    restartBluetooth(){
        this.setData({deviceList:[]});
        this.stopBluetoothSearch();
        this.searchBluetooth();
    },
    // 停止扫描
    stopBluetoothSearch(){
        this.setData({isSearch:false});
        // 停止扫描
        wx.stopBluetoothDevicesDiscovery();
        wx.offBluetoothAdapterStateChange((result) => {})
    },
    
    //打开蓝牙管理界面
    openBluetoothManagePage(e){
        var index=e.currentTarget.dataset.index;
        var deviceId= this.data.deviceList[index].deviceId;
        var deviceName=this.data.deviceList[index].name;
        var connectable=this.data.deviceList[index].connectable;
        if(!connectable){
            wx.showToast({
              title: '已被占用，不可连接',
              icon:"error"
            })
            return;
        }
        this.stopBluetoothSearch();
        wx.navigateTo({
          url: '/pages/bluetoothDevices/bluetoothDevicesManage?deviceId='+deviceId+'&deviceName='+deviceName,
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