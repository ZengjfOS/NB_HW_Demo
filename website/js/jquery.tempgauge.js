
(function($){
	$.fn.tempGauge = function(options) {
		var opts = $.extend({}, $.fn.tempGauge.defaults, options),
		padding = opts.borderWidth;			
		
		var gauges = [];
		
		this.each(function(idx, item){
			gauges.push(createTempGauge(item));
		});
		
		return $(gauges);
		
		function createTempGauge(gauge){
			
			var canvas = document.createElement("canvas"),
				ctx = canvas.getContext("2d"),
				currentTempText = $(gauge).text(),
				currentTemp = (( parseFloat(currentTempText) + 0.000001) || opts.defaultTemp).toFixed(1);

			canvas.width = opts.width;
			canvas.height = opts.width * 2 + opts.labelSize;

			if(opts.replaceHtml){
				$(gauge).replaceWith(canvas);
			}else{
				$(gauge).html(canvas);
			}

			var percentage = calculatePercentage(currentTemp, opts.minTemp, opts.maxTemp - opts.minTemp);

			ctx.lineWidth = opts.borderWidth;
			ctx.strokeStyle = opts.borderColor;
			ctx.fillStyle = opts.fillColor;
			ctx.font = "bold " + opts.labelSize + "px Arial ";
			ctx.textAlign = "center";
			
			fillTempGauge(ctx, 5, padding + padding, opts.width, opts.width * 2 - padding * 6, percentage);
			strokeTempGauge(ctx, 5, padding + padding, opts.width, opts.width * 2 - padding * 6);
			if (opts.showLabel) {
				drawLabel(ctx, canvas.width / 2, canvas.height - opts.labelSize / 5, currentTempText);
			}

			if(opts.showScale) {
				drawScale(ctx, 0, padding * 2, opts.width, opts.width * 2 - padding, 0, percentage, opts.maxTemp, opts.minTemp);
			}

			return canvas;
		}
		
		function drawScale(ctx, x, y, width, height, spacing, fillPercent, maxTemp, minTemp) {
			var longScale = 14, shortScale = 7;
			var o = calculateDrawArgs(x, y, width, height, spacing, fillPercent);
			var maxLength = height - o.offset - o.big.radius - o.small.radius;
			ctx.moveTo(x, y);
			ctx.lineTo(x, maxLength);
			ctx.stroke();

			var drawScaleLine = function (x, y, scaleLength) {
				ctx.moveTo(x, y);
				ctx.lineTo(x + scaleLength, y);
				ctx.stroke();
			};

			var delta = (maxLength + 0.001) / 10;
			for (var i = 0; i < 11; i++) {
				if (i % 2 === 0) {
					drawScaleLine(x, y + delta * i, longScale);
					drawLabel(ctx, x + longScale + 14, y + delta * i + 6, (maxTemp - (maxTemp - minTemp) * i/10).toString());
				} else {
					drawScaleLine(x, y + delta * i, shortScale);
					drawLabel(ctx, x + longScale + 14, y + delta * i + 6, (maxTemp - (maxTemp - minTemp) * i/10).toString());
				}
			}
		}

		function calculatePercentage(temp, mintemp, length){
			var percentage = (temp - mintemp)/ length;
			percentage = percentage > 1 ? 1 : percentage;
			percentage = percentage < 0 ? 0 : percentage;
			return percentage;
		}

		function calculateDrawArgs(x, y, width, height, spacing, fillPercent){

			var wholeCircle = Math.PI * 2;
			var smallRadius = width / 3 / 2 - spacing;
			var xSmall = x + width / 2;
			var ySmall = y + smallRadius + spacing;
			
			var bigRadius = height / 6 - spacing;
			var xBig = x + width / 2;
			var yBig = y + height / 6 * 5;
			
			var offSet = Math.sqrt((Math.pow(bigRadius, 2) - Math.pow(smallRadius / 2, 2)), 2);
			var twoThirdsLength = height / 6 * 5 - offSet - width / 3 / 2;

			var gauge = twoThirdsLength * fillPercent;
			
			var yBox = yBig - offSet - gauge;
			var xBox = xBig - width / 6 + spacing;
			var sRad = Math.asin(smallRadius / bigRadius);

			return {
				small : {x : xSmall, y : ySmall, radius : smallRadius},
				big : {x : xBig, y: yBig, radius : bigRadius},
				box : {x : xBox, y : yBox},
				offset : offSet,
				gauge : gauge,
				sRad : sRad,
				wholeCircle : wholeCircle
			};
		}
		
		function drawTemperatureGauge(ctx, x, y, width, height, spacing, fillPercent) {
			var o = calculateDrawArgs(x, y, width, height, spacing, fillPercent);
			ctx.beginPath();
			ctx.arc(o.small.x, o.box.y, o.small.radius, 0, o.wholeCircle * -0.5, true);
			ctx.arc(o.big.x, o.big.y, o.big.radius, o.wholeCircle * 0.75 - o.sRad, o.wholeCircle * -0.25 + o.sRad, true);
			ctx.closePath();
		}
		
		function strokeTempGauge(ctx, x, y, width, height){
			drawTemperatureGauge(ctx, x, y, width, height, 0, 1);
			ctx.stroke();
		}
		
		function fillTempGauge(ctx, x, y, width, height, percent){
			drawTemperatureGauge(ctx, x, y, width, height,  opts.borderWidth, percent);
			ctx.fill();
		}

		function drawLabel(ctx, x, y, text){
			ctx.fillStyle = opts.labelColor;
			ctx.fillText(text, x , y );
		}
	};
	
	$.fn.tempGauge.defaults = {
		borderColor: "black",
		borderWidth: 4,
		defaultTemp: 26,
		fillColor: "red",
		labelSize: 12,
		labelColor: "black",
		maxTemp: 40,
		minTemp: -10,
		showLabel: false,
		width: 100,
		showScale: false,
		replaceHtml: true
	};
		
})(jQuery);

/*	
    +---+
    |   |
    |   |
    | # |
  +-+ # +-+
  | ##### |
  | ##### |
  +-------+

*/
