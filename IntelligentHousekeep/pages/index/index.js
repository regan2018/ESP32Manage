const app=getApp();
Page({
  data: {
    PageCur: 'basics'
  },
  NavChange(e) {
    var url= e.currentTarget.dataset.url;
    wx.navigateTo({
      url: url,
    })
  },
  onShareAppMessage() {
    return {
      title: '智能管家',
      imageUrl: '/images/share.jpg',
      path: '/pages/index/index'
    }
  },
})