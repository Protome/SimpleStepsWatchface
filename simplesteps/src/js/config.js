module.exports = [
  {
    "type": "heading",
    "defaultValue": "Simple Steps Configuration"
  },
  {
    "type": "text",
    "defaultValue": "Set the colours for the watchface! If you have any issues (or just want to say hi,) message me @Protome on twitter"
  },
  {
    "type": "section",
    "items": [
      {
        "type": "heading",
        "defaultValue": "Colors"
      },
      {
        "type": "color",
        "messageKey": "BackgroundColour",
				"sunlight": false,
        "defaultValue": "0xFF5500",
        "label": "Background Color"
      },
      {
        "type": "color",
        "messageKey": "StripeColour",
				"sunlight": false,
        "defaultValue": "0xFFFFFF",
        "label": "Stripe Color"
      },
      {
        "type": "color",
        "messageKey": "BorderColour",
				"sunlight": false,
        "defaultValue": "0x000000",
        "label": "Border Color"
      },
      {
        "type": "color",
        "messageKey": "TimeColour",
				"sunlight": false,
        "defaultValue": "0x000000",
        "label": "Time Text Color"
      },
      {
        "type": "color",
        "messageKey": "StepsColour",
				"sunlight": false,
        "defaultValue": "0x000000",
        "label": "Steps Text Color"
      }
    ]
  },
	{
    "type": "section",
    "items": [
      {
        "type": "heading",
        "defaultValue": "More Settings"
      },
      {
        "type": "toggle",
        "messageKey": "StepsText",
				"label": "Show 'Steps:' before the number of steps",
        "defaultValue": true
      }
    ]
  },
  {
    "type": "submit",
    "defaultValue": "Save Settings"
  }
];